#pragma once
#include "Base.h"

BEGIN(Engine)

class CSound_Manager : public CBase
{
private:
	 CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();

public:
	void PlaySoundW(const string pSoundKey, CHANNELID eID, float fVolume, _bool _overlap = false);
	void PlayBGM(string pSoundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);

private:
	void	StoreAllSoundPath(const filesystem::path& CurrentPath, const string& FileExtension);
	void	FindAllSoundPath(const string& ModelPath, const string& FileExtension);

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<string, FMOD::Sound*> m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD :: Channel* m_pChannelArr[MAXCHANNEL];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD :: System* m_pSystem;

public:
	static CSound_Manager* Create();
	virtual void Free() override;

};

END