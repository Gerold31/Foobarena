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

#include "RobotTorso.hpp"
#include "RobotHead.hpp"
#include "RobotWeapon.hpp"

#include "RobotMovement.hpp"
#include "RobotMovementWheel.hpp"
#include "RobotMovementTrack.hpp"


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
}


void EntRobotT::Think(float FrameTime, unsigned long ServerFrameNr)
{

    string torsoID = "0", headID = "0", weaponID = "0", movementID = "0";
    int headCount = 0;
    int weaponCount = 0;
    int movementType = 0, movementCount = 0;

    for (std::map<std::string, std::string>::const_iterator It=Properties.begin(); It!=Properties.end(); ++It)
    {
        const std::string& key  =It->first;
        int value = atoi(It->second.c_str());

    //    Console->DevPrint("key: " + key + " value: " + It->second + "\n");

        if      (key=="TorsoID")        torsoID = It->second;
        else if (key=="HeadCount")      headCount = value;
        else if (key=="HeadID")         headID = It->second;
        else if (key=="WeaponCount")    weaponCount = value;
        else if (key=="WeaponID")       weaponID = It->second;
        else if (key=="MovementType")   movementType = value;
        else if (key=="MovementCount")  movementCount = value;
        else if (key=="MovementID")     movementID = It->second;
    }

    if(!mCreated)
    {
        mCreated = true;


        unsigned long id;
        std::map<std::string, std::string> propsTorso, propsHead, propsWeapon, propsMovement;

        propsTorso["classname"]="RobotTorso";
        //propsHead["PartID"]=torsoID;
        propsTorso["model"]="Models/Robot/robot_torso_" + torsoID + ".cmdl";
        id  = GameWorld->CreateNewEntity(propsTorso, ServerFrameNr, State.Origin);
    //    mTorso = (EntRobotTorsoT *)GameWorld->GetBaseEntityByID(id);

        propsHead["classname"]="RobotHead";
        //propsHead["PartID"]=headID;
        propsHead["model"]="Models/Robot/robot_head_" + headID + ".cmdl";
        for(int i=0; i<headCount; i++)
        {
       //     id = GameWorld->CreateNewEntity(propsHead, ServerFrameNr, State.Origin);
       //     ((EntRobotPartT *)GameWorld->GetBaseEntityByID(id))->loadModel(string("Games/Foobarena/Models/Robot/robot_head_") + /*char(rand()%1 + 0x32)*/ headID + ".cmdl");
            mHead.push_back((EntRobotHeadT *)GameWorld->GetBaseEntityByID(id));
        }

        /*
        propsWeapon["classname"]="RobotWeapon";
        propsWeapon["PartID"]=weaponID;
        for(int i=0; i<weaponCount; i++)
        {
            id = GameWorld->CreateNewEntity(propsWeapon, ServerFrameNr, State.Origin);
            mWeapon.push_back((EntRobotWeaponT *)GameWorld->GetBaseEntityByID(id));
        }
        */


        //propsMovement["PartID"]=movementID;
        for(int i=0; i<movementCount; i++)
        {
            switch(movementType)
            {
            case 0:
                propsMovement["classname"]="RobotMovementWheel";
                propsMovement["model"]="Models/Robot/robot_movement_wheel" + movementID + ".cmdl";
                break;
            case 1:
                propsMovement["classname"]="RobotMovementTrack";
                propsMovement["model"]="Models/Robot/robot_movement_track" + movementID + ".cmdl";
                break;
            }
            id = GameWorld->CreateNewEntity(propsMovement, ServerFrameNr, State.Origin);
            mMovement.push_back((EntRobotMovementT *)GameWorld->GetBaseEntityByID(id));
        }
        Console->DevPrint("Robot constructor finsihed\n");
    }

}


void EntRobotT::Draw(bool /*FirstPersonView*/, float LodDist) const
{
    for (std::map<std::string, std::string>::const_iterator It=Properties.begin(); It!=Properties.end(); ++It)
    {
        const std::string& key  =It->first;

     //   Console->DevPrint("key: " + key + " value: " + It->second + "\n");
    }
}


void EntRobotT::PostDraw(float FrameTime, bool /*FirstPersonView*/)
{
}


void EntRobotT::TakeDamage(BaseEntityT* Entity, char Amount, const VectorT& ImpactDir)
{
}
