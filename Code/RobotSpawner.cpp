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

#include "RobotSpawner.hpp"
#include "EntityCreateParams.hpp"
#include "PhysicsWorld.hpp"
#include "../../GameWorld.hpp"
#include "ConsoleCommands/Console.hpp"
#include "TypeSys.hpp"
#include "SceneGraph/Node.hpp"
#include "TelaString.hpp"
#include "ScriptState.hpp"
#include "MaterialSystem/Renderer.hpp"
#include "Math3D/Matrix.hpp"
#include "SceneGraph/Node.hpp"

extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

#define MAX_TORSO    3
#define MAX_HEAD     2
#define MAX_WEAPON   2
#define MAX_MOVEMENT 2


#define PRINT_VAR(x) Console->DevPrint((TelaString(#x) + " :" + (x) + "\n").toString())

static const luaL_reg MethodsList[]=
{
    { "spawnRobot", EntRobotSpawnerT::spawnRobot},
    { NULL, NULL }
};

// Implement the type info related code.
const cf::TypeSys::TypeInfoT* EntRobotSpawnerT::GetType() const
{
    return &TypeInfo;
 // return &EntRobotSpawnerT::TypeInfo;
}

void* EntRobotSpawnerT::CreateInstance(const cf::TypeSys::CreateParamsT& Params)
{
    return new EntRobotSpawnerT(*static_cast<const EntityCreateParamsT*>(&Params));
}

const cf::TypeSys::TypeInfoT EntRobotSpawnerT::TypeInfo(GetBaseEntTIM(), "EntRobotSpawnerT", "BaseEntityT", EntRobotSpawnerT::CreateInstance, MethodsList);


EntRobotSpawnerT::EntRobotSpawnerT(const EntityCreateParamsT& Params)
    : BaseEntityT(Params, EntityStateT(Params.Origin,
                                       VectorT(),
                                       BoundingBox3T<double>(VectorT( 7000.0,  7000.0,  7000.0),
                                                             VectorT(-7000.0, -7000.0, -7000.0)),
                                       0,       // Heading
                                       0,       // Pitch
                                       0,       // Bank
                                       0,       // StateOfExistance
                                       0,       // Flags
                                       0,       // ModelIndex
                                       0,       // ModelSequNr
                                       0.0,     // ModelFrameNr
                                       0,       // Health
                                       0,       // Armor
                                       0,       // HaveItems
                                       0,       // HaveWeapons
                                       0,       // ActiveWeaponSlot
                                       0,       // ActiveWeaponSequNr
                                       0.0))    // ActiveWeaponFrameNr)
{
}

void EntRobotSpawnerT::Think(float /*FrameTime*/, unsigned long ServerFrameNr)
{
    for(int i=0; i<mRobotsToSpawn.size(); i++)
    {
        Vector3dT pos;


        pos = State.Origin + Vector3dT((rand()%int(State.Dimensions.Max.x - State.Dimensions.Min.x))  - (State.Dimensions.Max.x - State.Dimensions.Min.x)/2,
                                       (rand()%int(State.Dimensions.Max.y - State.Dimensions.Min.y))  - (State.Dimensions.Max.y - State.Dimensions.Min.y)/2, 0);
        pos.z = 0;


        unsigned int id = GameWorld->CreateNewEntity(mRobotsToSpawn.at(i)->toProperties(), ServerFrameNr, pos);
        GameWorld->GetBaseEntityByID(id)->State.Origin = pos;
        delete mRobotsToSpawn.at(i);
    }
    mRobotsToSpawn.clear();
}


int EntRobotSpawnerT::spawnRobot(lua_State *l)
{
    EntRobotSpawnerT* Ent=(EntRobotSpawnerT*)cf::GameSys::ScriptStateT::GetCheckedObjectParam(l, 1, TypeInfo);

    Ent->mRobotsToSpawn.push_back(new RobotInfoT(luaL_checknumber(l, 2), luaL_checknumber(l, 3), luaL_checknumber(l, 4), luaL_checknumber(l, 5)));
    return 0;
}


EntRobotSpawnerT::RobotInfoT::RobotInfoT(int torsoID, int headID, int weaponID, int movementID)
{
    mTorsoID    = torsoID    < 0 ? (rand()%MAX_TORSO)    +1 : torsoID;
    mHeadID     = headID     < 0 ? (rand()%MAX_HEAD)     +1 : headID;
    mWeaponID   = weaponID   < 0 ? (rand()%MAX_WEAPON)   +1 : weaponID;
    mMovementID = movementID < 0 ? (rand()%MAX_MOVEMENT) +1 : movementID;
}

map<string, string> EntRobotSpawnerT::RobotInfoT::toProperties()
{
    map<string, string> props;
    props["classname"] = "Robot";
//    props["name"]      = "Robot";
    props["TorsoID"]   = TelaString(mTorsoID);
    props["HeadID"]    = TelaString(mHeadID);
    props["WeaponID"]  = TelaString(mWeaponID);
    props["MovementID"]= TelaString(mMovementID);
    return props;
}

