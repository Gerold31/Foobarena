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

#ifndef _ROBOTPART_HPP_
#define _ROBOTPART_HPP_

#include "../../BaseEntity.hpp"
#include "Models/AnimExpr.hpp"

class CafuModelT;
class EntityCreateParamsT;

class EntRobotT;

#include <string>
using namespace std;


class EntRobotPartT : public BaseEntityT
{
public:
    enum RobotPartType
    {
        RobotPartTorso,
        RobotPartHead,
        RobotPartWeapon,
        RobotPartMovement
    };

    EntRobotPartT(const EntityCreateParamsT& Params);
    ~EntRobotPartT();

    void Think(float FrameTime, unsigned long ServerFrameNr);
    void Draw(bool FirstPersonView, float LodDist) const;
    void PostDraw(float FrameTime, bool FirstPersonView);
    void TakeDamage(BaseEntityT* Entity, char Amount, const VectorT& ImpactDir);

    void Serialize(cf::Network::OutStreamT& Stream) const;
    void Deserialize(cf::Network::InStreamT& Stream, bool IsIniting=false);
    void Cl_UnserializeFrom();

    const cf::TypeSys::TypeInfoT* GetType() const;
    static void* CreateInstance(const cf::TypeSys::CreateParamsT& Params);
    static const cf::TypeSys::TypeInfoT TypeInfo;

    void setParent(EntRobotT *parent) {mParent = parent;}
    void setPartType(int type) {mType = type;}
    int getPartType() {return mType;}

    bool playAnimation(string name);

protected:
    CafuModelT* mModel;
    string mModelName, mCollisionModelName;
    EntRobotT *mParent;
    int mType;

    char  &mSequNr;
    mutable IntrusivePtrT<AnimExpressionT>   mAnimExpr;
    mutable IntrusivePtrT<AnimExprStandardT> mLastStdAE;
};

#endif
