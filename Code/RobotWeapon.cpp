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


// Implement the type info related code.
const cf::TypeSys::TypeInfoT* EntRobotWeaponT::GetType() const
{
    return &TypeInfo;
 // return &EntRobotWeaponT::TypeInfo;
}

void* EntRobotWeaponT::CreateInstance(const cf::TypeSys::CreateParamsT& Params)
{
    return NULL;
}

const cf::TypeSys::TypeInfoT EntRobotWeaponT::TypeInfo(GetBaseEntTIM(), "EntRobotWeaponT", "BaseEntityT", EntRobotWeaponT::CreateInstance, NULL /*MethodsList*/);


EntRobotWeaponT::EntRobotWeaponT(const EntityCreateParamsT &Params, const EntityStateT &State_)
    : BaseEntityT(Params, State_)
{
}

void EntRobotWeaponT::Think(float FrameTime, unsigned long /*ServerFrameNr*/)
{
}


void EntRobotWeaponT::Draw(bool /*FirstPersonView*/, float LodDist) const
{
    AnimPoseT* Pose=mModel->GetSharedPose(mModel->GetAnimExprPool().GetStandard(State.ModelSequNr, State.ModelFrameNr));
    Pose->Draw(-1 /*default skin*/, LodDist);
}


void EntRobotWeaponT::PostDraw(float FrameTime, bool /*FirstPersonView*/)
{
    IntrusivePtrT<AnimExprStandardT> StdAE=mModel->GetAnimExprPool().GetStandard(State.ModelSequNr, State.ModelFrameNr);

    StdAE->SetForceLoop(true);
    StdAE->AdvanceTime(FrameTime);

    State.ModelFrameNr=StdAE->GetFrameNr();
}


void EntRobotWeaponT::TakeDamage(BaseEntityT* Entity, char Amount, const VectorT& ImpactDir)
{
}
