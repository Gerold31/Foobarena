#include "Weapon.hpp"

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

#include "TelaString.hpp"

#define PRINT_VAR(x) Console->DevPrint((TelaString(#x) + " :" + x + "\n").toString())

#define WEAPON_PATH(s) (std::string("Games/Foobarena/Models/Weapons/") + s + "/" + s + "_v.cmdl")
#define WEAPON_DEFAULT "DesertEagle"

WeaponT::WeaponT(ModelManagerT *modelManager, std::string weaponName, char weaponId) : mWeaponId(weaponId)
{
    PRINT_VAR(weaponName);
	std::ifstream f(WEAPON_PATH(weaponName).c_str());
	if(!f.is_open())
	{
        Console->Warning(std::string("Couldn't find Weapon \"") + weaponName + "\" - falling back to WEAPON_DEFAULT");
		weaponName = WEAPON_DEFAULT;
	}
    mWeaponModel = modelManager->GetModel(WEAPON_PATH(weaponName));

    modelManager->GetModel(std::string("Games/Foobarena/Models/Items/Ammo_") + weaponName + "/Ammo_" + weaponName + ".cmdl");
	
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
    EntityStateT &state = Player->State;

    switch (state.ActiveWeaponSequNr)
    {
        case 3: // Reload
            if (AnimSequenceWrap)
            {
                state.ActiveWeaponSequNr =0;
                state.ActiveWeaponFrameNr=0.0;

                const char Amount=state.HaveAmmo[mWeaponId]>6 ? 6 : state.HaveAmmo[mWeaponId]; /// @todo use ammo slot instead of weapon slot?

                state.HaveAmmoInWeapons[mWeaponId]+=Amount;
                state.HaveAmmo         [mWeaponId]-=Amount; /// @todo use ammo slot instead of weapon slot?
            }
            break;

        case 4: // Holster
            break;

        case 5: // Draw
            if (AnimSequenceWrap)
            {
                // Back to idle.
                state.ActiveWeaponSequNr =0;
                state.ActiveWeaponFrameNr=0.0;
            }
            break;

        case 2: // Fire
            if (!AnimSequenceWrap) break;

            // Back to idle.
            state.ActiveWeaponSequNr =0;
            state.ActiveWeaponFrameNr=0.0;
            // Intentional fall-through.

        case 0: // Idle 1
        case 6: // Idle 2
        case 7: // Idle 3
        case 1: // Fidget 1
        {
            // 1. First see if the magazine is empty and special action is required.
            if (!state.HaveAmmoInWeapons[mWeaponId])
            {
                if (state.HaveAmmo[mWeaponId])
                {
                    state.ActiveWeaponSequNr =3;    // Reload
                    state.ActiveWeaponFrameNr=0.0;
                    break;
                }

                if (PlayerCommand.Keys & PCK_Fire1)
                {
                    // TODO: State.Events^=(1 << EventID_PrimaryFireEmpty);     // BUT LIMIT THE "FREQUENCY" OF THIS EVENT!
                    break;
                }
            }

            // 2. Are we to fire a bullet (we have at least one)?
            if (PlayerCommand.Keys & PCK_Fire1)
            {
                state.ActiveWeaponSequNr =2;                // Fire
                state.ActiveWeaponFrameNr=0.0;
                state.HaveAmmoInWeapons[mWeaponId]--;
                state.Events^=(1 << EntHumanPlayerT::EventID_PrimaryFire);   // Flip event flag.

                if (ThinkingOnServerSide)
                {
                    // If we are on the server-side, find out what or who we hit.
                    const float ViewDirZ=-LookupTables::Angle16ToSin[state.Pitch];
                    const float ViewDirY= LookupTables::Angle16ToCos[state.Pitch];

                    const VectorT ViewDir(ViewDirY*LookupTables::Angle16ToSin[state.Heading], ViewDirY*LookupTables::Angle16ToCos[state.Heading], ViewDirZ);

                    RayResultT RayResult(Player->GetRigidBody());
                    Player->PhysicsWorld->TraceRay(state.Origin/1000.0, scale(ViewDir, 9999999.0/1000.0), RayResult);

                    if (RayResult.hasHit() && RayResult.GetHitEntity()!=NULL)
                        RayResult.GetHitEntity()->TakeDamage(Player, 7, ViewDir);
                }
                break;
            }

            // 3. If nothing else has happened, just choose another sequence number on sequence wrap.
            if (AnimSequenceWrap)
            {
                const char RandomNumber=char(LookupTables::RandomUShort[PlayerCommand.Nr & 0xFFF]);

                     if (RandomNumber< 96) state.ActiveWeaponSequNr=0;  // Idle 1
                else if (RandomNumber<192) state.ActiveWeaponSequNr=6;  // Idle 2
                else if (RandomNumber<224) state.ActiveWeaponSequNr=7;  // Idle 3
                else                       state.ActiveWeaponSequNr=1;  // Fidget 1

                state.ActiveWeaponFrameNr=0.0;
            }
            break;
        }
    }
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
    const float MaxAge=3.0f;
	
	particle->Age+=time;
	if (particle->Age>MaxAge) return false;
	
	const float p=1.0f-particle->Age/MaxAge;     // % of remaining lifetime
	
	particle->Color[0]=char( 20.0f*p);
    particle->Color[1]=char(180.0f*p);
    particle->Color[2]=char(255.0f*p);
	
	return true;
}

bool WeaponT::ParticleFunction_HitEntity(ParticleMST *particle, float time)
{
    const float MaxAge=1.0f;

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
