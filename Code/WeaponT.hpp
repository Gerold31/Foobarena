#ifndef CAFU_WEAPONT_HPP_INCLUDED
#define CAFU_WEAPONT_HPP_INCLUDED

#include <cstring>
#include <fstream>

#include "Math3D/Vector3.hpp"


class  BaseEntityT;
class  EntHumanPlayerT;
struct PlayerCommandT;
class  CafuModelT;
class  ModelManagerT;
class  SoundI;
struct ParticleMST;


class WeaponT
{
public:
    WeaponT(ModelManagerT *modelManager, std::string weaponModel, char weaponId);
	virtual ~WeaponT();
	
	const CafuModelT* getWeaponModel() const;
	
	virtual void ServerSide_Think(EntHumanPlayerT* Player, const PlayerCommandT& PlayerCommand, bool ThinkingOnServerSide, unsigned long ServerFrameNr, bool AnimSequenceWrap) const;
	
	virtual void ClientSide_HandleFireEvent(const EntHumanPlayerT* Player, const VectorT& LastSeenAmbientColor) const;
	virtual void ClientSide_HandleStateDrivenEffects(const EntHumanPlayerT* Player) const;
	
	
private:
	const CafuModelT *mWeaponModel;
	SoundI *mShotSound;
    const char mWeaponId;
	
	static bool ParticleFunction_ShotHitWall(ParticleMST *particle, float time);
	static bool ParticleFunction_HitEntity(ParticleMST *particle, float time);
	static bool ParticleFunction_ShotWhiteSmoke(ParticleMST *particle, float time);
};


#endif
