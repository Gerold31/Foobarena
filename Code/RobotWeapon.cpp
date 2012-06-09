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

#include "RobotWeapon.hpp"
#include "TypeSys.hpp"
#include "EntityCreateParams.hpp"
#include "Libs/LookupTables.hpp"
#include "../../GameWorld.hpp"
#include "Models/Model_cmdl.hpp"
#include "ConsoleCommands/Console.hpp"

// Implement the type info related code.
const cf::TypeSys::TypeInfoT* EntRobotWeaponT::GetType() const
{
    return &TypeInfo;
 // return &EntRobotWeaponT::TypeInfo;
}

void* EntRobotWeaponT::CreateInstance(const cf::TypeSys::CreateParamsT& Params)
{
    return new EntRobotWeaponT(*static_cast<const EntityCreateParamsT*>(&Params));
}

const cf::TypeSys::TypeInfoT EntRobotWeaponT::TypeInfo(GetBaseEntTIM(), "EntRobotWeaponT", "BaseEntityT", EntRobotWeaponT::CreateInstance, NULL /*MethodsList*/);


EntRobotWeaponT::EntRobotWeaponT(const EntityCreateParamsT& Params)
    : EntRobotPartT(Params, string("Games/Foobarena/Models/Robot/robot_weapon_") + Params.Properties.find("PartID")->second + ".cmdl")
{
}

