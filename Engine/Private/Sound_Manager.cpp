#include "Sound_Manager.h"

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize()
{
	// 사운드를 담당하는 대표객체를 생성하는 함수
	FMOD :: System_Create(&m_pSystem);
	
	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	m_pSystem->init(32, FMOD_INIT_NORMAL, NULL);

	FindAllSoundPath("../../Data/Sound/", ".wav");

	return S_OK;
}

void CSound_Manager::PlaySoundW(const string pSoundKey, CHANNELID eID, float fVolume, _bool _overlap)
{
	auto iter = m_mapSound.find(pSoundKey);

	if (iter == m_mapSound.end())
		return;

	_bool bPlaying = false;

	if (_overlap)
		m_pSystem->playSound(iter->second, nullptr,  false, &m_pChannelArr[eID]);

	else
	{
		if (m_pChannelArr[eID]->isPlaying(&bPlaying))
		{
			m_pSystem->playSound(iter->second, nullptr, false, &m_pChannelArr[eID]);
		}
		
	}
	m_pChannelArr[eID]->setVolume(fVolume);

	m_pSystem->update();
}

void CSound_Manager::PlayBGM(string pSoundKey, float fVolume)
{
	auto iter = m_mapSound.find(pSoundKey);

	if (iter == m_mapSound.end())
		return;

	m_pSystem->playSound(iter->second, nullptr, FALSE, &m_pChannelArr[SOUND_BGM]);
	m_pChannelArr[SOUND_BGM]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[SOUND_BGM]->setVolume(fVolume);
	m_pSystem->update();
}

void CSound_Manager::StopSound(CHANNELID eID)
{
	m_pChannelArr[eID]->stop();
}

void CSound_Manager::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		m_pChannelArr[i]->stop();
}

void CSound_Manager::SetChannelVolume(CHANNELID eID, float fVolume)
{
	m_pChannelArr[eID]->setVolume(fVolume);
	m_pSystem->update();
}


void CSound_Manager::StoreAllSoundPath(const filesystem::path& CurrentPath, const string& FileExtension)
{
	for (const auto& entry : filesystem::directory_iterator(CurrentPath))
	{
		if (filesystem::is_regular_file(entry) && entry.path().extension() == FileExtension)
		{
			FMOD::Sound* pSound = nullptr;

			string strPath	=	entry.path().stem().string();
			string Test		=	entry.path().string();

			FMOD_RESULT eRes = m_pSystem->createSound(entry.path().string().c_str(), FMOD_LOOP_OFF, 0, &pSound);

			if (eRes == FMOD_OK)
			{
				m_mapSound.emplace(strPath, pSound);
			}
		}

		else if (filesystem::is_directory(entry))
		{
			StoreAllSoundPath(entry.path(), FileExtension);
		}
	}
}

void CSound_Manager::FindAllSoundPath(const string& ModelPath, const string& FileExtension)
{
	filesystem::path fullPath = ModelPath;
	
	StoreAllSoundPath(fullPath, FileExtension);
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();
	
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CSound_Manager");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CSound_Manager::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		//delete[] Mypair.first;
		Mypair.second->release();
	}
	m_mapSound.clear();

	m_pSystem->release();
	m_pSystem->close();
}
