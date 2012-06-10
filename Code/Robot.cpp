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

#include "File.hpp"

#include "RobotTorso.hpp"
#include "RobotHead.hpp"
#include "RobotWeapon.hpp"

#include "RobotMovement.hpp"
#include "RobotMovementWheel.hpp"
#include "RobotMovementTrack.hpp"

#define PRINT_VAR(x) Console->DevPrint((TelaString(#x) + " :" + x + "\n").toString())

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
    State.Heading = 0;
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


        // --------------------------------Torso---------------------------------

        File *file = new File("Games/Foobarena/Models/Robot/robot_torso_" + torsoID + ".cfg");
        file->open();
        torsoHealth = file->getInt("health");
        speedFactor = file->getDouble("speedfactor");

        mHeadCount = file->getInt("slotheadcount");
        for(int i=0; i<mHeadCount; i++)
        {
            mSlots.push_back(Vector3T<double>(file->getDouble(TelaString("slotheadposx") + i),
                                              file->getDouble(TelaString("slotheadposy") + i),
                                              file->getDouble(TelaString("slotheadposz") + i)));
        }
        mWeaponCount = file->getInt("slotweaponcount");
        for(int i=0; i<mWeaponCount; i++)
        {
            mSlots.push_back(Vector3T<double>(file->getDouble(TelaString("slotweaponx") + i),
                                              file->getDouble(TelaString("slotweapony") + i),
                                              file->getDouble(TelaString("slotweaponz") + i)));
        }
        mMovementCount = file->getInt("slotmovemntcount");
        for(int i=0; i<mMovementCount; i++)
        {
            mSlots.push_back(Vector3T<double>(file->getDouble(TelaString("slotmovementx") + i),
                                              file->getDouble(TelaString("slotmovementy") + i),
                                              file->getDouble(TelaString("slotmovementz") + i)));
        }

        propsTorso["classname"]="RobotTorso";
        propsTorso["model"]="Models/Robot/robot_torso_" + torsoID + ".cmdl";
        //propsTorso["collisionModel"]="Models/Robot/robot_torso_" + torsoID + ".cmdl";

        id  = GameWorld->CreateNewEntity(propsTorso, ServerFrameNr, State.Origin);
        mTorso = (EntRobotTorsoT *)GameWorld->GetBaseEntityByID(id);
        mTorso->State.Health = torsoHealth;

        delete file;


        // --------------------------------Head---------------------------------

        file = new File("Games/Foobarena/Models/Robot/robot_head_" + headID + ".cfg");
        file->open();

        headHealth = file->getInt("health");
        mRange = file->getInt("range");

        propsHead["classname"]="RobotHead";
        propsHead["model"]="Models/Robot/robot_head_" + headID + ".cmdl";
        //propsHead["collisionModel"]="Models/Robot/robot_head_" + headID + ".cmdl";
        for(int i=0; i<mHeadCount; i++)
        {
            id = GameWorld->CreateNewEntity(propsHead, ServerFrameNr, State.Origin);
            GameWorld->GetBaseEntityByID(id)->State.Health = headHealth;
            mHead.push_back((EntRobotHeadT *)GameWorld->GetBaseEntityByID(id));
        }

        delete file;


        // --------------------------------Weapon---------------------------------

        file = new File("Games/Foobarena/Models/Robot/robot_weapon_" + weaponID + ".cfg");
        file->open();

        weaponHealth = file->getInt("health");
        mDamage = file->getInt("damage");
        mFirerate = file->getDouble("firerate");

        propsWeapon["classname"]="RobotWeapon";
        propsWeapon["model"]="Models/Robot/robot_weapon_" + weaponID + ".cmdl";
        //propsWeapon["collisionModel"]="Models/Robot/robot_weapon_" + weaponID + ".cmdl";
        for(int i=0; i<mWeaponCount; i++)
        {
            id = GameWorld->CreateNewEntity(propsWeapon, ServerFrameNr, State.Origin);
            GameWorld->GetBaseEntityByID(id)->State.Health = weaponHealth;
            mWeapon.push_back((EntRobotWeaponT *)GameWorld->GetBaseEntityByID(id));
        }

        delete file;


        // --------------------------------Movement---------------------------------

        switch(movementType)
        {
        case 0:
            file = new File("Games/Foobarena/Models/Robot/robot_movement_wheel_" + movementID + ".cfg");
            propsMovement["classname"]="RobotMovementWheel";
            propsMovement["model"]="Models/Robot/robot_movement_wheel_" + movementID + ".cmdl";
            //propsMovement["collisionModel"]="Models/Robot/robot_movement_wheel_" + movementID + ".cmdl";
            break;
        case 1:
            file = new File("Games/Foobarena/Models/Robot/robot_movement_track_" + movementID + ".cfg");
            propsMovement["classname"]="RobotMovementTrack";
            propsMovement["model"]="Models/Robot/robot_movement_track_" + movementID + ".cmdl";
            //propsMovement["collisionModel"]="Models/Robot/robot_movement_track_" + movementID + ".cmdl";
            break;
        }

        file->open();
        movementHealth = file->getInt("health");
        mSpeed = file->getInt("speed");
        mSpeed = (int)(speedFactor * mSpeed);
        mMovementRadius = file->getInt("radius");

        for(int i=0; i<mMovementCount; i++)
        {
            id = GameWorld->CreateNewEntity(propsMovement, ServerFrameNr, State.Origin);
            GameWorld->GetBaseEntityByID(id)->State.Health = movementHealth;
            mMovement.push_back((EntRobotMovementT *)GameWorld->GetBaseEntityByID(id));
        }

        delete file;
    }

    //------------------------------Positioning----------------------------------
    //-------------------------------Torso--------------------------------------
    mTorso->State.Origin = State.Origin + Vector3T<double>(0,0,mMovementRadius);
    PRINT_VAR( mTorso->State.Origin.z);
    mTorso->State.Heading = State.Heading;

    //-------------------------------Head--------------------------------------
    int j=0;
    for(int i=0; i<mHeadCount; i++, j++)
    {
        mHead.at(i)->State.Origin = mTorso->State.Origin + mSlots.at(j).GetRotZ(State.Heading);
        mHead.at(i)->State.Heading = State.Heading;
    }

    //-------------------------------Weapon--------------------------------------
    for(int i=0; i<mWeaponCount; i++, j++)
    {
        mWeapon.at(i)->State.Origin = mTorso->State.Origin + mSlots.at(j).GetRotZ(State.Heading);
        if(i%2 == 0)
            mWeapon.at(i)->State.Heading = State.Heading;
        else
        {
            mWeapon.at(i)->State.Heading = -State.Heading;
            mWeapon.at(i)->State.Pitch = 1 << 15;
        }

        PRINT_VAR(mWeapon.at(i)->State.Bank);
    }

    //-----------------------------Movement--------------------------------------
    for(int i=0; i<mMovementCount; i++, j++)
    {
        mMovement.at(i)->State.Origin = mTorso->State.Origin + mSlots.at(j).GetRotZ(State.Heading);
        if(i%2 == 0)
            mMovement.at(i)->State.Heading = State.Heading;
        else
        {
            mMovement.at(i)->State.Heading = -State.Heading;
            mMovement.at(i)->State.Pitch = 1 << 15;
        }
    }

}


void EntRobotT::Draw(bool /*FirstPersonView*/, float LodDist) const
{
}


void EntRobotT::PostDraw(float FrameTime, bool /*FirstPersonView*/)
{
}


void EntRobotT::TakeDamage(BaseEntityT* Entity, char Amount, const VectorT& ImpactDir)
{
}
