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
	// ���� ���ҽ� ������ ���� ��ü 
	map<string, FMOD::Sound*> m_mapSound;

	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD :: Channel* m_pChannelArr[MAXCHANNEL];

	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD :: System* m_pSystem;

public:
	static CSound_Manager* Create();
	virtual void Free() override;

};

END