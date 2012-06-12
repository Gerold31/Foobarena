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

#ifndef _SMOKE_HPP_
#define _SMOKE_HPP_

#include "BaseEntity.hpp"
#include "Math3D/Vector3.hpp"

class EntityCreateParamsT;
struct ParticleMST;

class EntSmokeT : public BaseEntityT
{
public:
    EntSmokeT(const EntityCreateParamsT& Params);

    void Think(float FrameTime, unsigned long ServerFrameNr);
    void Draw(bool FirstPersonView, float LodDist) const;
    void PostDraw(float FrameTime, bool FirstPersonView);

    const cf::TypeSys::TypeInfoT* GetType() const;
    static void* CreateInstance(const cf::TypeSys::CreateParamsT& Params);
    static const cf::TypeSys::TypeInfoT TypeInfo;

protected:
    int mStartSize, mEndSize;
    int mColor[4];
    int mNumberOfParicles;
    double mLifeTime, mParticleSpawnTime, mElapsedTime, mTimeSinceLastParticle;
    Vector3dT mParticleVelocity;

    static bool ParticleFunction(ParticleMST *particle, float time);
    void createParticle();
};

#endif
