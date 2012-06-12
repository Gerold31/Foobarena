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

#include "Smoke.hpp"
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
#include "_ResourceManager.hpp"
#include "ParticleEngine/ParticleEngineMS.hpp"

#include <string>
#include <map>
using namespace std;

// Implement the type info related code.
const cf::TypeSys::TypeInfoT* EntSmokeT::GetType() const
{
    return &TypeInfo;
 // return &EntSmokeT::TypeInfo;
}

void* EntSmokeT::CreateInstance(const cf::TypeSys::CreateParamsT& Params)
{
    return new EntSmokeT(*static_cast<const EntityCreateParamsT*>(&Params));
}

const cf::TypeSys::TypeInfoT EntSmokeT::TypeInfo(GetBaseEntTIM(), "EntSmokeT", "BaseEntityT", EntSmokeT::CreateInstance, NULL /*MethodsList*/);


EntSmokeT::EntSmokeT(const EntityCreateParamsT& Params)
    : BaseEntityT(Params, EntityStateT(Params.Origin,
                                       VectorT(),
                                       BoundingBox3T<double>(VectorT( 100.0,  100.0,  100.0),
                                                             VectorT(-100.0, -100.0, -100.0)),
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
    for(map<string, string>::const_iterator i=Properties.begin(); i!=Properties.end(); ++i)
    {
        string key   = (*i).first;
        string value = (*i).second;

        if(key == "StartSize")
        {
            mStartSize = atoi(value.c_str());
        }else if(key == "EndSize")
        {
            mEndSize = atoi(value.c_str());
        }else if(key == "NumberOfParticles")
        {
            mNumberOfParicles = atoi(value.c_str());
        }else if(key == "LifeTime")
        {
            mLifeTime = atof(value.c_str());
        }else if(key == "ParticleSpawnTime")
        {
            mParticleSpawnTime = atof(value.c_str());
        }else if(key == "Color")
        {
            std::istringstream iss(value);
            iss >> mColor[0] >> mColor[1] >> mColor[2] >> mColor[3];
        }else if(key == "ParticleVelocity")
        {
            std::istringstream iss(value);
            iss >> mParticleVelocity[0] >> mParticleVelocity[1] >> mParticleVelocity[2];
        }
    }
    mElapsedTime = mTimeSinceLastParticle = 0;
    createParticle();
}

void EntSmokeT::Think(float FrameTime, unsigned long /*ServerFrameNr*/)
{
    mElapsedTime += FrameTime;
    mTimeSinceLastParticle += FrameTime;
    if(mElapsedTime >= mLifeTime && mLifeTime > 0)
    {
        GameWorld->RemoveEntity(ID);
        return;
    }
    if(mTimeSinceLastParticle > mParticleSpawnTime)
    {
        createParticle();
        mTimeSinceLastParticle = 0;
    }
}


void EntSmokeT::Draw(bool /*FirstPersonView*/, float LodDist) const
{
}


void EntSmokeT::PostDraw(float FrameTime, bool FirstPersonView)
{
}

bool EntSmokeT::ParticleFunction(ParticleMST *particle, float time)
{
    particle->Age+=time;

    particle->Origin[0] += particle->Velocity[0] * time;
    particle->Origin[1] += particle->Velocity[1] * time;
    particle->Origin[2] += particle->Velocity[2] * time;

    particle->Radius += particle->AuxData[0] * time;

    if(particle->Color[3] - particle->AuxData[2] * time > 0)
        particle->Color[3] -= particle->AuxData[2] * time;
    else particle->Color[3] = 0;


    particle->RenderMat=ResMan.RenderMats[ResMan.PARTICLE_WHITESMOKE_FRAME1+((unsigned long)(particle->Age*20))%2 + 14];

    if (particle->Age>=particle->AuxData[1]) return false;

    return true;
}

void EntSmokeT::createParticle()
{
    if(State.Origin == Vector3dT()) return;
    ParticleMST NewParticle;

    NewParticle.Origin[0] = State.Origin.x;
    NewParticle.Origin[1] = State.Origin.y;
    NewParticle.Origin[2] = State.Origin.z;

    NewParticle.Velocity[0] = mParticleVelocity[0];
    NewParticle.Velocity[1] = mParticleVelocity[1];
    NewParticle.Velocity[2] = mParticleVelocity[2];

    NewParticle.Age = 0.0;
    NewParticle.Color[0] = mColor[0];
    NewParticle.Color[1] = mColor[1];
    NewParticle.Color[2] = mColor[2];
    NewParticle.Color[3] = mColor[3];

    NewParticle.Radius = mStartSize;
    NewParticle.StretchY = 1.0;
    NewParticle.RenderMat = ResMan.RenderMats[ResMan.PARTICLE_WHITESMOKE_FRAME1];
    NewParticle.MoveFunction = &EntSmokeT::ParticleFunction;

    NewParticle.AuxData[0] = (mEndSize - mStartSize)/(mParticleSpawnTime * mNumberOfParicles);
    NewParticle.AuxData[1] = mParticleSpawnTime * mNumberOfParicles;
    NewParticle.AuxData[2] = mColor[3]/(mParticleSpawnTime * mNumberOfParicles);

    ParticleEngineMS::RegisterNewParticle(NewParticle);
}
