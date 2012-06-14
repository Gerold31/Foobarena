/*
=================================================================================
This file is part of Cafu, the open-source game engine and graphics engine
for multiplayer, cross-platform, real-time 3D action.
Copyright (C) 2002-2012 Carsten Fuchs Software.

Cafu is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

Cafu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Cafu. If not, see <http://www.gnu.org/licenses/>.

For support and more information about Cafu, visit us at <http://www.cafu.de>.
=================================================================================
*/

#include "Robot.hpp"
#include "TypeSys.hpp"
#include "EntityCreateParams.hpp"
#include "Libs/LookupTables.hpp"
#include "../../GameWorld.hpp"
#include "Models/Model_cmdl.hpp"
#include "ConsoleCommands/Console.hpp"
#include "Math3D/Matrix.hpp"
#include "ParticleEngine/ParticleEngineMS.hpp"
#include "_ResourceManager.hpp"
#include "SoundSystem/SoundSys.hpp"
#include "SoundSystem/SoundShaderManager.hpp"
#include "SoundSystem/Sound.hpp"
#include "PhysicsWorld.hpp"
#include "File.hpp"
#include "ClipSys/ClipModel.hpp"
#include "ClipSys/ClipWorld.hpp"
#include "ClipSys/TraceResult.hpp"
#include "MaterialSystem/Material.hpp"

#include "RobotPart.hpp"
#include "Smoke.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

#define PRINT_VAR(x) Console->DevPrint((TelaString(#x) + " :" + (x) + "\n").toString())

#define ARENASIZE 45000.0

#define LENGTH(a) (sqrt((a).x*(a).x + (a).y*(a).y))

#define ROT_SPEED (mSpeed * FrameTime *3)

// Implement the type info related code.
const cf::TypeSys::TypeInfoT* EntRobotT::GetType() const
{
    return &TypeInfo;
 // return &EntRobotT::TypeInfo;
}

void* EntRobotT::CreateInstance(const cf::TypeSys::CreateParamsT& Params)
{
    return new EntRobotT(*static_cast<const EntityCreateParamsT*>(&Params));
}

const cf::TypeSys::TypeInfoT EntRobotT::TypeInfo(GetBaseEntTIM(), "EntRobotT", "BaseEntityT", EntRobotT::CreateInstance, NULL /*MethodsList*/);


EntRobotT::EntRobotT(const EntityCreateParamsT& Params)
    : BaseEntityT(Params,
                  EntityStateT(Params.Origin,          // Beachte die Abhängigkeit von den in Think() definierten Konstanten!
                               VectorT(),
                               BoundingBox3T<double>(VectorT( 100.0,  100.0,  100.0),
                                                     VectorT(-100.0, -100.0, -100.0)),
                               0,                                           // Beachte die Abhängigkeit von den in Think() definierten Konstanten!
                               0,
                               0,
                               0,
                               0,
                               0,       // ModelIndex
                               0,       // ModelSequNr
                               0.0,     // ModelFrameNr
                               20,      // Health
                               0,       // Armor
                               0,       // HaveItems
                               0,       // HaveWeapons
                               0,       // ActiveWeaponSlot
                               0,       // ActiveWeaponSequNr
                               0.0))    // ActiveWeaponFrameNr
{
    mCreated = false;
    State.Origin.z = 0;
    State.Heading = 1<<14;
    mSmoke = NULL;
    mTimeSinceLastShot = 0;
    mIsShooting = false;

    mTarget = NULL;
    ArrayT<unsigned long> ids = GameWorld->GetAllEntityIDs();
    for(int i=0; i<ids.Size(); i++)
    {
        if(GameWorld->GetBaseEntityByID(ids[i])->Name == "Player1")
        {
            mTarget = GameWorld->GetBaseEntityByID(ids[i]);
            break;
        }
    }
}

EntRobotT::~EntRobotT()
{
    for(int i=0; i<mPart.size(); i++)
    {
        if(mPart.at(i))
            GameWorld->RemoveEntity(mPart.at(i)->ID);
    }
    if(mSmoke)
        GameWorld->RemoveEntity(mSmoke->ID);
    mSmoke = NULL;
    /*
    if(mSound)
        SoundSystem->DeleteSound(mSound);
    */
}

void EntRobotT::Think(float FrameTime, unsigned long ServerFrameNr)
{
    if(!mCreated)
    {
        mCreated = true;

        string torsoID = "0", headID = "0", weaponID = "0", movementID = "0";
        int movementType = 0;

        int torsoHealth = 100, headHealth = 50, weaponHealth = 50, movementHealth = 50;
        double speedFactor = 1;

        unsigned long id;
        std::map<std::string, std::string> propsTorso, propsHead, propsWeapon, propsMovement;

        for (std::map<std::string, std::string>::const_iterator It=Properties.begin(); It!=Properties.end(); ++It)
        {
            const std::string& key  =It->first;

        //    Console->DevPrint("key: " + key + " value: " + It->second + "\n");

            if      (key=="TorsoID")        torsoID = It->second;
            else if (key=="HeadID")         headID = It->second;
            else if (key=="WeaponID")       weaponID = It->second;
            else if (key=="MovementType")   movementType = atoi(It->second.c_str());
            else if (key=="MovementID")     movementID = It->second;
        }

        EntRobotPartT *part;

        // --------------------------------Torso---------------------------------

        File *file = new File("Games/Foobarena/Models/Robot/robot_torso_" + torsoID + ".cfg");
        file->open();
        torsoHealth = file->getInt("health");
        speedFactor = file->getDouble("speedfactor");

        // torso is always at 0,0,0
        mSlotPos.push_back(Vector3dT(0,0,0));

        mSlotRot.push_back(Vector3dT(file->getDouble(TelaString("slottorsorotp")),
                                     file->getDouble(TelaString("slottorsoroty")),
                                     file->getDouble(TelaString("slottorsorotr"))));

        mHeadCount = file->getInt("slotheadcount");
        for(int i=0; i<mHeadCount; i++)
        {
            mSlotPos.push_back(Vector3dT(file->getDouble(TelaString("slotheadposx") + i),
                                         file->getDouble(TelaString("slotheadposy") + i),
                                         file->getDouble(TelaString("slotheadposz") + i)));

            mSlotRot.push_back(Vector3dT(file->getDouble(TelaString("slotheadrotp") + i),
                                         file->getDouble(TelaString("slotheadroty") + i),
                                         file->getDouble(TelaString("slotheadrotr") + i)));
        }
        mWeaponCount = file->getInt("slotweaponcount");
        for(int i=0; i<mWeaponCount; i++)
        {
            mSlotPos.push_back(Vector3dT(file->getDouble(TelaString("slotweaponposx") + i),
                                         file->getDouble(TelaString("slotweaponposy") + i),
                                         file->getDouble(TelaString("slotweaponposz") + i)));

            mSlotRot.push_back(Vector3dT(file->getDouble(TelaString("slotweaponrotp") + i),
                                         file->getDouble(TelaString("slotweaponroty") + i),
                                         file->getDouble(TelaString("slotweaponrotr") + i)));
        }
        mMovementCount = file->getInt("slotmovementcount");
        for(int i=0; i<mMovementCount; i++)
        {
            mSlotPos.push_back(Vector3dT(file->getDouble(TelaString("slotmovementposx") + i),
                                         file->getDouble(TelaString("slotmovementposy") + i),
                                         file->getDouble(TelaString("slotmovementposz") + i)));

            mSlotRot.push_back(Vector3dT(file->getDouble(TelaString("slotmovementrotp") + i),
                                         file->getDouble(TelaString("slotmovementroty") + i),
                                         file->getDouble(TelaString("slotmovementrotr") + i)));
        }

        propsTorso["classname"]     ="RobotPart";
        propsTorso["name"]          =TelaString("Torso") + Name;
        propsTorso["model"]         ="Models/Robot/robot_torso_" + torsoID + ".cmdl";
        propsTorso["collisionModel"]="Models/Robot/robot_torso_" + torsoID + ".cmap";

        id  = GameWorld->CreateNewEntity(propsTorso, ServerFrameNr, State.Origin);
        part = (EntRobotPartT *)GameWorld->GetBaseEntityByID(id);
        part->State.Health = torsoHealth;
        part->setParent(this);
        part->setPartType(EntRobotPartT::RobotPartTorso);
        mPart.push_back(part);

        delete file;


        // --------------------------------Head---------------------------------

        file = new File("Games/Foobarena/Models/Robot/robot_head_" + headID + ".cfg");
        file->open();

        headHealth = file->getInt("health");
        mRange     = file->getInt("range");
        mFovy      = file->getInt("fovy");

        propsHead["classname"]      ="RobotPart";
        propsHead["model"]          ="Models/Robot/robot_head_" + headID + ".cmdl";
        propsHead["collisionModel"] ="Models/Robot/robot_head_" + headID + ".cmap";
        for(int i=0; i<mHeadCount; i++)
        {
            propsHead["name"] = TelaString("Head") + i + Name;
            id = GameWorld->CreateNewEntity(propsHead, ServerFrameNr, State.Origin);
            part = (EntRobotPartT *)GameWorld->GetBaseEntityByID(id);
            part->State.Health = headHealth;
            part->setParent(this);
            part->setPartType(EntRobotPartT::RobotPartHead);
            mPart.push_back(part);
        }

        delete file;


        // --------------------------------Weapon---------------------------------

        file = new File("Games/Foobarena/Models/Robot/robot_weapon_" + weaponID + ".cfg");
        file->open();

        weaponHealth = file->getInt("health");
        mDamage = file->getInt("damage");
        mFirerate = file->getDouble("firerate");
        mSound = SoundSystem->CreateSound3D(SoundShaderManager->GetSoundShader(file->getStr("sound").c_str()));
        if(mSound)
        {
            mSound->SetInnerConeAngle(90);
            mSound->SetInnerVolume(1.0);
            mSound->SetOuterConeAngle(180);
            mSound->SetOuterVolume(0.5);
        }

        propsWeapon["classname"]     ="RobotPart";
        propsWeapon["model"]         ="Models/Robot/robot_weapon_" + weaponID + ".cmdl";
        propsWeapon["collisionModel"]="Models/Robot/robot_weapon_" + weaponID + ".cmap";
        for(int i=0; i<mWeaponCount; i++)
        {
            propsWeapon["name"] = TelaString("Weapon") + i + Name;
            id = GameWorld->CreateNewEntity(propsWeapon, ServerFrameNr, State.Origin);
            part = (EntRobotPartT *)GameWorld->GetBaseEntityByID(id);
            part->State.Health = weaponHealth;
            part->setParent(this);
            part->setPartType(EntRobotPartT::RobotPartWeapon);
            part->State.Velocity.x = file->getDouble("barrelposx");
            part->State.Velocity.y = file->getDouble("barrelposy");
            part->State.Velocity.z = file->getDouble("barrelposz");
            mPart.push_back(part);
        }

        delete file;


        // --------------------------------Movement---------------------------------

        file = new File("Games/Foobarena/Models/Robot/robot_movement_" + movementID + ".cfg");
        propsMovement["classname"]      ="RobotPart";
        propsMovement["model"]          ="Models/Robot/robot_movement_" + movementID + ".cmdl";
        propsMovement["collisionModel"] ="Models/Robot/robot_movement_" + movementID + ".cmap";

        file->open();
        movementHealth = file->getInt("health");
        mSpeed = file->getInt("speed");
        mSpeed = (int)(speedFactor * mSpeed);
        mMovementRadius = file->getInt("radius");

        for(int i=0; i<mMovementCount; i++)
        {
            propsMovement["name"] = TelaString("Movement") + i + Name;
            id = GameWorld->CreateNewEntity(propsMovement, ServerFrameNr, State.Origin);
            part = (EntRobotPartT *)GameWorld->GetBaseEntityByID(id);
            part->State.Health = movementHealth;
            part->setParent(this);
            part->setPartType(EntRobotPartT::RobotPartMovement);
            mPart.push_back(part);
        }

        delete file;
    }

    if(!mPart.size()) return;
    if(!mPart.at(0)) return;

    //------------------------------Positioning----------------------------------
    int movementCount = 0, headCount = 0, weaponCount = 0;

    mTimeSinceLastShot += FrameTime;

    for(int i=0; i<mPart.size(); i++)
    {
        if(!mPart.at(i)) continue;
        if(mPart.at(i)->getPartType() == EntRobotPartT::RobotPartHead)
            headCount++;
        else if(mPart.at(i)->getPartType() == EntRobotPartT::RobotPartWeapon)
            weaponCount++;
        else if(mPart.at(i)->getPartType() == EntRobotPartT::RobotPartMovement)
            movementCount++;
    }

    if(mPart.at(0)->State.Health > 0)
    {
        Vector3dT dir = mTarget->State.Origin - State.Origin;

        /*
        dir = dir.GetRotZ(State.Heading*45.0/8192.0);
        dir.y = -dir.y;
        */

        double playerAngle = atan2(dir.y, dir.x)* 180.0 / M_PI + State.Heading *45.0f/8192.0f;
        while(playerAngle < -180) playerAngle += 360; while(playerAngle >=180)playerAngle -= 360;

        if(movementCount / (double)mMovementCount > 0.5)
        {
            Vector3dT oldPos = State.Origin;
            bool inSpawn = false;

            if(LENGTH(State.Origin) > ARENASIZE)
            {
                // get out of the spawn
                inSpawn = true;
                double angle = atan2(-State.Origin.y, State.Origin.x)* 180.0 / M_PI;

                angle += 180;
                while(angle < -180) angle += 360; while(angle >= 180)angle -= 360;

                /*
                if(angle > State.Heading*45.0f/8192.0f)
                    State.Heading -= ROT_SPEED;
                else State.Heading += ROT_SPEED;
                */
                State.Heading = angle*8192/45.0;
            }
            if(!mIsShooting || inSpawn)
                State.Origin += Vector3dT((mDirection || inSpawn)? (mSpeed * FrameTime) : -(mSpeed * FrameTime),0,0).GetRotZ(-State.Heading *45.0f/8192.0f);

            State.Velocity = State.Origin - oldPos;
        }else if(movementCount > 0)
        {
            for(int i=0; i<mPart.size(); i++)
            {
                if(!mPart.at(i)) continue;
                if(mPart.at(i)->getPartType() == EntRobotPartT::RobotPartMovement)
                {
                    GameWorld->RemoveEntity(mPart.at(i)->ID);
                    mPart[i] = NULL;
                }
            }
            movementCount = 0;
        }

        if(headCount > 0)
        {
            if(weaponCount > 0 && mTarget)
            {
                // head to player
                if(LENGTH(dir) < mRange/3*1000 && (playerAngle > -90 && playerAngle < 90))
                {
                    mDirection = false;
                    if(playerAngle >= 0)
                        State.Heading -= ROT_SPEED;
                    else
                        State.Heading += ROT_SPEED;

                }else
                {
                    mDirection = true;

                    if(playerAngle >= 0)
                        State.Heading -= ROT_SPEED;
                    else
                        State.Heading += ROT_SPEED;
                }



                if(playerAngle < 2 && playerAngle > -2 && LENGTH(dir) <= mRange*1000)
                {
                    // shoot
                    mIsShooting = true;

                    if(mTimeSinceLastShot >= 1.0/mFirerate)
                    {

                        for(int i=0; i<mPart.size(); i++)
                        {
                            if(!mPart.at(i)) continue;
                            if(mPart.at(i)->getPartType() == EntRobotPartT::RobotPartWeapon)
                            {
                                mPart.at(i)->playAnimation("Shot");

                                //Console->DevPrint((char *)(TelaString("Velocity: ") + mPart.at(i)->State.Velocity.x + " " + mPart.at(i)->State.Velocity.y + "  " + mPart.at(i)->State.Velocity.z + "\n"));
                                Vector3dT pos = mPart.at(i)->State.Velocity;
                                pos.z = 0;
                                pos = mPart.at(i)->State.Origin + pos.GetRotY(mSlotRot.at(i).y).GetRotZ(90-mSlotRot.at(i).z-mPart.at(i)->State.Heading *45.0f/8192.0f)*25;
                                pos.z += mPart.at(i)->State.Velocity.z*25;
                                //Console->DevPrint((char *)(TelaString("Shooting: FrameTime:") + FrameTime + " timeSinceLastShot: " + mTimeSinceLastShot + " NextShot: " + (double)1.0/mFirerate + " FireRate: " + mFirerate + "\n"));

                                //create smoking barrel effect
                                std::map<std::string, std::string> props;
                                props["classname"] = "Smoke";
                                props["StartSize"] = TelaString(mDamage).c_str();
                                props["EndSize"] = TelaString(10*mDamage).c_str();
                                props["LifeTime"] = TelaString(0.1/mFirerate).c_str();
                                props["ParticleSpawnTime"] = "0.01";
                                props["Color"] = "127 127 127 255";
                                props["NumberOfParticles"] = TelaString((0.1/mFirerate)/0.001).c_str();
                                props["ParticleVelocity"] = "0 0 2000";

                                unsigned long id = GameWorld->CreateNewEntity(props, ServerFrameNr, pos);
                                EntSmokeT *smoke = (EntSmokeT *)GameWorld->GetBaseEntityByID(id);
                                smoke->State.Origin = pos;

                                //play sound
                                if(mSound)
                                {
                                    mSound->SetPosition(pos);
                                    mSound->SetDirection(pos - mPart.at(i)->State.Origin);
                                    mSound->SetVelocity(State.Velocity);
                                    mSound->Play();
                                }

                                const unsigned short Pitch   = mPart.at(i)->State.Pitch  ;//+(rand() % 2000)-1000;
                                const unsigned short Heading = mPart.at(i)->State.Heading+(rand() % 500)-250 + 90*8192.0/45.0;

                                const float ViewDirZ = -LookupTables::Angle16ToSin[Pitch];
                                const float ViewDirY =  LookupTables::Angle16ToCos[Pitch];

                                VectorT ViewDir(ViewDirY*LookupTables::Angle16ToSin[Heading], ViewDirY*LookupTables::Angle16ToCos[Heading], ViewDirZ);
                                ViewDir *= mRange *1000;

                                cf::ClipSys::TraceResultT RayResult;
                                cf::ClipSys::ClipModelT *hitClipModel = new cf::ClipSys::ClipModelT(GameWorld->GetClipWorld());

                                GameWorld->GetClipWorld().TraceRay(State.Origin, ViewDir, MaterialT::Clip_Projectiles, &mPart.at(i)->ClipModel, RayResult, &hitClipModel);

                                if(hitClipModel)
                                {
                                    BaseEntityT *ent = (BaseEntityT *)hitClipModel->GetUserData();
                                    if(ent)
                                        ent->TakeDamage((BaseEntityT *)mPart.at(i), mDamage, ViewDir);
                                }
                            }
                        }
                        mTimeSinceLastShot = 0;
                    }
                }else
                {
                    mIsShooting = false;
                    for(int i=0; i<mPart.size(); i++)
                    {
                        if(!mPart.at(i)) continue;
                        if(mPart.at(i)->getPartType() == EntRobotPartT::RobotPartWeapon)
                            mPart.at(i)->playAnimation("");
                    }
                }

            }
        }

        // debug
        //mPart.at(0)->State.Health--;
    }else
    {
        if(!mSmoke)
        {
            std::map<std::string, std::string> props;
            props["classname"] = "Smoke";
            props["StartSize"] = "1500";
            props["EndSize"] = "10000";
            props["LifeTime"] = "0.5";
            props["ParticleSpawnTime"] = "0.01";
            props["Color"] = "127 127 127 255";
            props["NumberOfParticles"] = "700";
            props["ParticleVelocity"] = "0 0 2000";

            unsigned long id = GameWorld->CreateNewEntity(props, ServerFrameNr, State.Origin);
            mSmoke = (EntSmokeT *)GameWorld->GetBaseEntityByID(id);
        }
        State.Origin.z -= 2000*FrameTime;
        if(State.Origin.z < -4000)
        {
            GameWorld->RemoveEntity(ID);
        }
    }


    for(int i=0; i<mPart.size(); i++)
    {
        if(!mPart.at(i)) continue;
        mPart.at(i)->State.Origin  = State.Origin + Vector3dT(0,0, movementCount > 0 ? mMovementRadius : 0) + mSlotPos.at(i).GetRotZ(-State.Heading *45.0f/8192.0f);
        mPart.at(i)->State.Pitch   = mSlotRot.at(i).x*8192.0f/45.0f;
        mPart.at(i)->State.Bank    = mSlotRot.at(i).y*8192.0f/45.0f;
        mPart.at(i)->State.Heading = mSlotRot.at(i).z*8192.0f/45.0f + State.Heading;
        if(mPart.at(i)->getPartType() == EntRobotPartT::RobotPartTorso && mIsShooting == false)
            mPart.at(i)->playAnimation("");
    }

    if(mSmoke)
    {
        mSmoke->State.Origin = State.Origin;
    }
}

void EntRobotT::TakeDamage(BaseEntityT* Entity, char Amount, const VectorT& ImpactDir, bool isTorso, EntRobotPartT *part)
{
    if(isTorso)
    {
        // 60% damage to torso, rest to the other parts
        part->State.Health -= 0.6* Amount;
        int damage = 0.4 * Amount/ mPart.size();
        for(int i=1; i<mPart.size(); i++)
        {
            if(!mPart.at(i)) continue;
            mPart.at(i)->State.Health -= damage;
            // do not destroy part
            if(mPart.at(i)->State.Health < 1)
                mPart.at(i)->State.Health = 1;
        }
    }else
    {
        // 60% damage to part, rest to torso
        part->State.Health -= 0.6*Amount;
        mPart.at(0)->State.Health -= 0.4*Amount;
        if(part->State.Health <= 0)
        {
            /// @todo add some smoke effects
            /// @todo create ridgit Body and accelerate in ImpactDir
            // find part;
            for(int i=1; i<mPart.size(); i++)
            {
                if(!mPart.at(i)) continue;
                if(mPart.at(i) == part)
                {
                    GameWorld->RemoveEntity(mPart.at(i)->ID);
                    mPart[i] = NULL;
                    break;
                }
            }
        }
    }
}
