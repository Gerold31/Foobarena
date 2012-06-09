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

#include "RobotPart.hpp"
#include "EntityCreateParams.hpp"
#include "PhysicsWorld.hpp"
#include "SoundSystem/SoundSys.hpp"
#include "SoundSystem/Sound.hpp"
#include "SoundSystem/SoundShaderManager.hpp"
#include "../../GameWorld.hpp"
#include "ConsoleCommands/Console.hpp"
#include "Models/Model_cmdl.hpp"
#include "TypeSys.hpp"


// Implement the type info related code.
const cf::TypeSys::TypeInfoT* EntRobotPartT::GetType() const
{
    return &TypeInfo;
 // return &EntRobotPartT::TypeInfo;
}

void* EntRobotPartT::CreateInstance(const cf::TypeSys::CreateParamsT& Params)
{
    Console->Warning("Cannot instantiate abstract class!\n");
    assert(false);
    return NULL;
}

const cf::TypeSys::TypeInfoT EntRobotPartT::TypeInfo(GetBaseEntTIM(), "EntRobotPartT", "BaseEntityT", EntRobotPartT::CreateInstance, NULL /*MethodsList*/);


EntRobotPartT::EntRobotPartT(const EntityCreateParamsT& Params, const std::string& ModelName)
    : BaseEntityT(Params,
                  EntityStateT(Params.Origin,
                               VectorT(),
                               BoundingBox3T<double>(VectorT( 200.0,  200.0,  400.0),
                                                     VectorT(-200.0, -200.0, -100.0)),
                               0,                           // Heading
                               0,                           // Pitch
                               0,                           // Bank
                               0,     						// StateOfExistance
                               0,                           // Flags
                               0,                           // ModelIndex
                               0,                           // ModelSequNr
                               0.0,                         // ModelFrameNr
                               10,                           // Health
                               0,                           // Armor
                               0,                           // HaveRobotParts
                               0,                           // HaveWeapons
                               0,                           // ActiveWeaponSlot
                               0,                           // ActiveWeaponSequNr
                               0.0)),                       // ActiveWeaponFrameNr
      mModel(GameWorld->GetModel(ModelName))
{
    Console->DevPrint(mModel->GetFileName()+"\n");
}

EntRobotPartT::~EntRobotPartT()
{
    Console->DevPrint("~EntRobotPartT()\n");
}

void EntRobotPartT::Think(float FrameTime, unsigned long /*ServerFrameNr*/)
{
}


void EntRobotPartT::Draw(bool /*FirstPersonView*/, float LodDist) const
{
    AnimPoseT* Pose=mModel->GetSharedPose(mModel->GetAnimExprPool().GetStandard(0, 0.0f));
    Pose->Draw(-1 /*default skin*/, LodDist);
}


void EntRobotPartT::PostDraw(float FrameTime, bool FirstPersonView)
{
}


void EntRobotPartT::TakeDamage(BaseEntityT* Entity, char Amount, const VectorT& ImpactDir)
{
}
