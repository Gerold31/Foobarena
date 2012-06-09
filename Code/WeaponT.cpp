#include "WeaponT.hpp"

#include "ConsoleCommands/Console.hpp"
#include "_ResourceManager.hpp"
#include "HumanPlayer.hpp"
#include "PhysicsWorld.hpp"
#include "Libs/LookupTables.hpp"
#include "../../GameWorld.hpp"
#include "Models/ModelManager.hpp"
#include "ParticleEngine/ParticleEngineMS.hpp"
#include "SoundSystem/SoundSys.hpp"
#include "SoundSystem/SoundShaderManager.hpp"
#include "SoundSystem/Sound.hpp"


#define WEAPON_PATH(s) (std::string("Games/Foobarena/Models/Weapons/") + s + "/" + s + "_v.cmdl")

WeaponT::WeaponT(ModelManagerT *modelManager, std::string weaponName)
{
	std::ifstream f(WEAPON_PATH(weaponName).c_str());
	if(!f.is_open())
	{
		Console->Warning(std::string("Couldn't find Weapon \"") + weaponName + "\" - falling back to Baretta!");
		weaponName = "Baretta";
	}
	mWeaponModel = modelManager->GetModel(WEAPON_PATH(weaponName));
	
	mShotSound = SoundSystem->CreateSound3D(SoundShaderManager->GetSoundShader(std::string("Weapon/") + weaponName + "_Shot"));
	
}

WeaponT::~WeaponT()
{
	SoundSystem->DeleteSound(mShotSound);
}

const CafuModelT* WeaponT::getWeaponModel() const
{
	return mWeaponModel;
}

void WeaponT::ServerSide_Think(EntHumanPlayerT* Player, const PlayerCommandT& PlayerCommand, bool ThinkingOnServerSide, unsigned long ServerFrameNr, bool AnimSequenceWrap) const
{

}

void WeaponT::ClientSide_HandleFireEvent(const EntHumanPlayerT* Player, const VectorT& LastSeenAmbientColor) const
{
	const EntityStateT& State = Player->State;
	
	const unsigned short Pitch   = State.Pitch  +(rand() % 200)-100;
	const unsigned short Heading = State.Heading+(rand() % 200)-100;
	
	const float ViewDirZ = -LookupTables::Angle16ToSin[Pitch];
	const float ViewDirY =  LookupTables::Angle16ToCos[Pitch];
	
	const VectorT ViewDir(ViewDirY*LookupTables::Angle16ToSin[Heading], ViewDirY*LookupTables::Angle16ToCos[Heading], ViewDirZ);
	
	RayResultT RayResult(Player->GetRigidBody());
	Player->PhysicsWorld->TraceRay(State.Origin/1000.0, scale(ViewDir, 9999999.0/1000.0), RayResult);
	
	if(RayResult.hasHit())
	{
		// Register a new particle at the 'Hit' point.
		ParticleMST NewParticle;
		
		NewParticle.Origin[0] = RayResult.m_hitPointWorld.x()*1000.0f;
		NewParticle.Origin[1] = RayResult.m_hitPointWorld.y()*1000.0f;
		NewParticle.Origin[2] = RayResult.m_hitPointWorld.z()*1000.0f;
		
		NewParticle.Velocity[0] = 0;
		NewParticle.Velocity[1] = 0;
		NewParticle.Velocity[2] = 0;
		
		NewParticle.Age = 0.0;
		NewParticle.Color[3] = 0;
		
		NewParticle.Radius = 300.0;
		NewParticle.StretchY = 1.0;
		NewParticle.RenderMat = ResMan.RenderMats[ResMan.PARTICLE_GENERIC1];
		NewParticle.MoveFunction = RayResult.GetHitEntity() == NULL ? WeaponT::ParticleFunction_ShotHitWall : WeaponT::ParticleFunction_HitEntity;
		
		ParticleEngineMS::RegisterNewParticle(NewParticle);
	}
	
	// Register a new particle as "muzzle flash".
	ParticleMST NewParticle;
	
	NewParticle.Origin[0] = float(State.Origin.x+ViewDir.x*400.0);
	NewParticle.Origin[1] = float(State.Origin.y+ViewDir.y*400.0);
	NewParticle.Origin[2] = float(State.Origin.z+ViewDir.z*400.0-100.0);
	
	NewParticle.Velocity[0] = float(ViewDir.x*1500.0);
	NewParticle.Velocity[1] = float(ViewDir.y*1500.0);
	NewParticle.Velocity[2] = float(ViewDir.z*1500.0);
	
	NewParticle.Age = 0.0;
	NewParticle.Color[0] = char(LastSeenAmbientColor.x*255.0);
	NewParticle.Color[1] = char(LastSeenAmbientColor.y*255.0);
	NewParticle.Color[2] = char(LastSeenAmbientColor.z*255.0);
	NewParticle.Color[3] = 180;
	
	NewParticle.Radius = 200.0;
	NewParticle.Rotation = char(rand());
	NewParticle.StretchY = 1.0;
	NewParticle.RenderMat = ResMan.RenderMats[ResMan.PARTICLE_WHITESMOKE_FRAME1];
	NewParticle.MoveFunction = ParticleFunction_ShotWhiteSmoke;
	
	ParticleEngineMS::RegisterNewParticle(NewParticle);
	
	// Update sound position and velocity.
	mShotSound->SetPosition(State.Origin+scale(ViewDir, 400.0));
	mShotSound->SetVelocity(State.Velocity);
	
	// Play the fire sound.
	mShotSound->Play();
}

void WeaponT::ClientSide_HandleStateDrivenEffects(const EntHumanPlayerT* Player) const
{

}


bool WeaponT::ParticleFunction_ShotHitWall(ParticleMST *particle, float time)
{
	const float MaxAge=0.4f;
	
	particle->Age+=time;
	if (particle->Age>MaxAge) return false;
	
	const float p=1.0f-particle->Age/MaxAge;     // % of remaining lifetime
	
	particle->Color[0]=char( 20.0f*p);
	particle->Color[1]=char(255.0f*p);
	particle->Color[2]=char(180.0f*p);
	
	return true;
}

bool WeaponT::ParticleFunction_HitEntity(ParticleMST *particle, float time)
{
	const float MaxAge=1.0;
	
	particle->Age+=time;
	if (particle->Age>MaxAge) return false;
	
	const float p=1.0f-particle->Age/MaxAge;     // % of remaining lifetime.
	
	particle->Color[0]=char(255.0f*p);
	particle->Color[1]=0;
	particle->Color[2]=0;
	
	return true;
}

bool WeaponT::ParticleFunction_ShotWhiteSmoke(ParticleMST *particle, float time)
{
	const float FPS   =20.0f;        // The default value is 20.0.
	const float MaxAge=32.0f/FPS;    // 32 frames at 20 FPS.
	
	particle->Origin[0]+=particle->Velocity[0]*time;
	particle->Origin[1]+=particle->Velocity[1]*time;
	particle->Origin[2]+=particle->Velocity[2]*time;
	
	particle->Radius+=time*1000.0f;
	particle->RenderMat=ResMan.RenderMats[ResMan.PARTICLE_WHITESMOKE_FRAME1+(unsigned long)(particle->Age*FPS)];
	
	particle->Age+=time;
	if (particle->Age>=MaxAge) return false;
	
	return true;
}
