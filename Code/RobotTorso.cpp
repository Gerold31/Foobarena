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

#include "RobotTorso.hpp"
#include "TypeSys.hpp"
#include "EntityCreateParams.hpp"
#include "Libs/LookupTables.hpp"
#include "../../GameWorld.hpp"
#include "Models/Model_cmdl.hpp"
#include "ConsoleCommands/Console.hpp"

// Implement the type info related code.
const cf::TypeSys::TypeInfoT* EntRobotTorsoT::GetType() const
{
    return &TypeInfo;
 // return &EntRobotTorsoT::TypeInfo;
}

void* EntRobotTorsoT::CreateInstance(const cf::TypeSys::CreateParamsT& Params)
{
    return new EntRobotTorsoT(*static_cast<const EntityCreateParamsT*>(&Params));
}

const cf::TypeSys::TypeInfoT EntRobotTorsoT::TypeInfo(GetBaseEntTIM(), "EntRobotTorsoT", "EntRobotPartT", EntRobotTorsoT::CreateInstance, NULL /*MethodsList*/);


EntRobotTorsoT::EntRobotTorsoT(const EntityCreateParamsT& Params)
    : EntRobotPartT(Params, true)
{
}

