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
#include "Network/State.hpp"
#include "MaterialSystem/Renderer.hpp"
#include "ClipSys/ClipModel.hpp"
#include "ClipSys/CollisionModelMan.hpp"
#include "TelaString.hpp"
#include "Robot.hpp"
#include "Math3D/Matrix3x3.hpp"
#include "Math3D/Angles.hpp"


#define PRINT_VAR(x) Console->DevPrint((TelaString(#x) + " :" + x + "\n").toString())

// Implement the type info related code.
const cf::TypeSys::TypeInfoT* EntRobotPartT::GetType() const
{
    return &TypeInfo;
 // return &EntRobotPartT::TypeInfo;
}

void* EntRobotPartT::CreateInstance(const cf::TypeSys::CreateParamsT& Params)
{
    return new EntRobotPartT(*static_cast<const EntityCreateParamsT*>(&Params));
}

const cf::TypeSys::TypeInfoT EntRobotPartT::TypeInfo(GetBaseEntTIM(), "EntRobotPartT", "BaseEntityT", EntRobotPartT::CreateInstance, NULL /*MethodsList*/);


EntRobotPartT::EntRobotPartT(const EntityCreateParamsT& Params)
    : BaseEntityT(Params, EntityStateT(Params.Origin,
                                       VectorT(),
                                       BoundingBox3T<double>(VectorT( 100.0,  100.0,  100.0),
                                                             VectorT(-100.0, -100.0, -100.0)),
                                       0,       // Heading
                                       0,       // Pitch
                                       0,       // Bank
                                       0,       // StateOfExistance
                                       1,       // Flags
                                       0,       // ModelIndex
                                       0,       // ModelSequNr
                                       0.0,     // ModelFrameNr
                                       0,       // Health
                                       0,       // Armor
                                       0,       // HaveItems
                                       0,       // HaveWeapons
                                       0,       // ActiveWeaponSlot
                                       0,       // ActiveWeaponSequNr
                                       0.0)),    // ActiveWeaponFrameNr)
      mSequNr(State.ModelSequNr),
      mAnimExpr(),
      mLastStdAE()
{
//    Console->DevPrint("EntRobotPartT::EntRobotPartT()\n");
    if(Params.Properties.count("model"))
    {
        mModelName = Params.Properties.find("model")->second;
 //       Console->DevPrint("model = " + mModelName + "\n");
        mModel = const_cast<CafuModelT *>(GameWorld->GetModel("Games/Foobarena/" + mModelName));
        mLastStdAE=mModel->GetAnimExprPool().GetStandard(mSequNr, 0.0f);
        mLastStdAE->SetForceLoop(false);
        mAnimExpr =mLastStdAE;
        mSequNr   =mLastStdAE->GetSequNr();
    }else
    {
//        Console->DevPrint("model = \"\"\n");
        mModelName = "";
        mModel = NULL;
    }

    if(Params.Properties.count("collisionModel"))
    {

        mCollisionModelName = Params.Properties.find("collisionModel")->second;
        CollisionModel=cf::ClipSys::CollModelMan->GetCM("Games/Foobarena/"+mCollisionModelName);
        ClipModel.SetOrigin(State.Origin);
        ClipModel.SetCollisionModel(CollisionModel);
        ClipModel.SetUserData(this);
        ClipModel.Register();
    }else
    {
        mCollisionModelName = "";
        //CollisionModel = NULL;
    }

    mParent = NULL;
    mType = -1;
}

EntRobotPartT::~EntRobotPartT()
{
//    Console->DevPrint("EntRobotPartT::~EntRobotPartT()\n");
    mModel = NULL;
}

void EntRobotPartT::Think(float FrameTime, unsigned long /*ServerFrameNr*/)
{
//    if(mType == RobotPartTorso)
    {
        ClipModel.SetOrigin(State.Origin);
        /*
        // TODO: Optimize! This matrix computation takes many unnecessary muls and adds...!
        ClipModel.SetOrientation(cf::math::Matrix3x3T<double>::GetRotateZMatrix(90.0-double(State.Heading)/8192.0*45.0)
                               * cf::math::Matrix3x3T<double>::GetRotateYMatrix(     double(State.Bank   )/8192.0*45.0)
                               * cf::math::Matrix3x3T<double>::GetRotateXMatrix(    -double(State.Pitch  )/8192.0*45.0));
        */
        ClipModel.SetOrientation(cf::math::Matrix3x3dT(cf::math::AnglesdT(-double(State.Pitch  )/8192.0*45.0, double(State.Bank   )/8192.0*45.0, 90.0-double(State.Heading)/8192.0*45.0)));
        ClipModel.Register();
    }
}


void EntRobotPartT::Draw(bool /*FirstPersonView*/, float LodDist) const
{
    if(!mModel) return;

    Vector3fT LgtPos(MatSys::Renderer->GetCurrentLightSourcePosition());
    Vector3fT EyePos(MatSys::Renderer->GetCurrentEyePosition());

    const float DegBank =float(State.Bank )/8192.0f*45.0f;
    const float DegPitch=float(State.Pitch)/8192.0f*45.0f;

    LgtPos=LgtPos.GetRotY(-DegBank);
    EyePos=EyePos.GetRotY(-DegBank);
    MatSys::Renderer->RotateY(MatSys::RendererI::MODEL_TO_WORLD, DegBank);

    LgtPos=LgtPos.GetRotX(DegPitch);
    EyePos=EyePos.GetRotX(DegPitch);
    MatSys::Renderer->RotateX(MatSys::RendererI::MODEL_TO_WORLD, -DegPitch);

    MatSys::Renderer->SetCurrentLightSourcePosition(LgtPos.x, LgtPos.y, LgtPos.z);
    MatSys::Renderer->SetCurrentEyePosition(EyePos.x, EyePos.y, EyePos.z);

    AnimPoseT* Pose=mModel->GetSharedPose(mAnimExpr);
    Pose->Draw(-1 /*default skin*/, LodDist);
}


void EntRobotPartT::PostDraw(float FrameTime, bool FirstPersonView)
{
    const int SequNr=(mSequNr==255) ? -1 : mSequNr;   // This is a hack, because mSequNr actually has the wrong (unsigned) datatype...

    if(SequNr == -1) return;

    if (SequNr != mLastStdAE->GetSequNr())
    {
  //      if (mPlayAnim)
        {
            mLastStdAE=mModel->GetAnimExprPool().GetStandard(SequNr, 0.0f);
            mLastStdAE->SetForceLoop(true);
            mAnimExpr =mModel->GetAnimExprPool().GetBlend(mAnimExpr, mLastStdAE, 3.0f);
        }
 //       else
        {
            mLastStdAE->SetSequNr(mSequNr);
        }

        mSequNr=mLastStdAE->GetSequNr();  // Set mSequNr to the mLastStdAE's "normalized" value.
    }

//    Console->DevPrint((char *)(TelaString("Client: ") + mModelName + (int)mPlayAnim + "\n"));

//    if (mPlayAnim)
    {
        // Advance the client-local animation.
        mAnimExpr->AdvanceTime(FrameTime);
    }
}


void EntRobotPartT::TakeDamage(BaseEntityT* Entity, char Amount, const VectorT& ImpactDir)
{
    if(mParent)
        mParent->TakeDamage(Entity, Amount, ImpactDir, mType == RobotPartTorso, this);
}

void EntRobotPartT::Serialize(cf::Network::OutStreamT& Stream) const
{
    Stream << float(State.Origin.x);
    Stream << float(State.Origin.y);
    Stream << float(State.Origin.z);
    Stream << State.Heading;
    Stream << State.Pitch;
    Stream << State.Bank;
    Stream << State.ModelIndex;
    Stream << State.ModelSequNr;
    Stream << State.ModelFrameNr;

    Stream << mModelName;
    Stream << mCollisionModelName;
}

void EntRobotPartT::Deserialize(cf::Network::InStreamT& Stream, bool IsIniting)
{
    float f;
    Stream >> f;State.Origin.x = f;
    Stream >> f;State.Origin.y = f;
    Stream >> f;State.Origin.z = f;
    Stream >> State.Heading;
    Stream >> State.Pitch;
    Stream >> State.Bank;
    Stream >> State.ModelIndex;
    Stream >> State.ModelSequNr;
    Stream >> State.ModelFrameNr;
    Stream >> mModelName;
    Stream >> mCollisionModelName;
    if(!mModel)
    {
//        Console->DevPrint("Deserialize: model = " + mModelName + "\n");
        mModel = const_cast<CafuModelT *>(GameWorld->GetModel("Games/Foobarena/" + mModelName));
        mLastStdAE=mModel->GetAnimExprPool().GetStandard(mSequNr, 0.0f);
        mLastStdAE->SetForceLoop(true);
        mAnimExpr =mLastStdAE;
        mSequNr   =mLastStdAE->GetSequNr();
    }
//    if(mType == RobotPartTorso)
    {
        if(!CollisionModel)
        {
//            Console->DevPrint("Deserialize: collision model = " + mCollisionModelName + "\n");
            CollisionModel=cf::ClipSys::CollModelMan->GetCM("Games/Foobarena/"+mCollisionModelName);
            ClipModel.SetCollisionModel(CollisionModel);
            ClipModel.SetUserData(this);
        }
    }
}

void EntRobotPartT::Cl_UnserializeFrom()
{
    ClipModel.SetOrigin(State.Origin);
    // TODO: Optimize! This matrix computation takes many unnecessary muls and adds...!
    /*
    ClipModel.SetOrientation(cf::math::Matrix3x3T<double>::GetRotateZMatrix(90.0-double(State.Heading)/8192.0*45.0)
                           * cf::math::Matrix3x3T<double>::GetRotateYMatrix(     double(State.Bank   )/8192.0*45.0)
                           * cf::math::Matrix3x3T<double>::GetRotateXMatrix(    -double(State.Pitch  )/8192.0*45.0));
    */
    ClipModel.SetOrientation(cf::math::Matrix3x3dT(cf::math::AnglesdT(-double(State.Pitch  )/8192.0*45.0, double(State.Bank   )/8192.0*45.0, 90.0-double(State.Heading)/8192.0*45.0)));
    ClipModel.Register();
}

bool EntRobotPartT::playAnimation(string name)
{
    if(name == "")
    {
        mSequNr = -1;
        return true;
    }
    ArrayT<CafuModelT::AnimT> anims = mModel->GetAnims();
    for(int i=0; i<anims.Size(); i++)
    {
        if(anims[i].Name == name)
        {
            mSequNr = i;
//            Console->DevPrint((char *)((TelaString("seq: ") + anims[i].Name + " Frames: " + (long)anims[i].Frames.Size() + "\n")));
            return true;
        }
    }
    return false;
}
