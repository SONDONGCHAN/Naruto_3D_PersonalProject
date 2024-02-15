#pragma once
#include "Base.h"
#include "Channel.h"


BEGIN(Engine)
class CAnimation : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(class CModel* pModel);
	void Invalidate_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _float fAnimationPlayRatio);
	void Change_Invalidate_TransformationMatrix(_float TimeAfterChange, _float changingDelay, const vector<class CBone*>& Bones, vector<_uint> CommonIndex, vector<CChannel::KEYFRAME> KeyFrames);


public:
	void	Set_AnimationName(_char* szName) { strcpy_s(m_szName, szName); }
	void	Set_Duration(_double duration) { m_Duration = duration;  }
	void	Set_TickPerSecond(_double tickpersecond) { m_TickPerSecond = tickpersecond; }
	void	Set_NumChannels(_uint numChannels) { m_iNumChannels = numChannels; }
	void	Set_VecChannels(vector<class CChannel*> vecChannels) { m_vecChannels = vecChannels; }

	void	Reset_TrackPosition();

	void	Set_CanStop() { m_CanStop = false; }
	void	Set_Finished() { m_bFinished = false; }
	void	Set_Loop(_bool bLoop)	{ m_Loop = bLoop; }		// ������ ������
	void	Set_TrackPosition() { m_TrackPosition = 0.f; }




	_uint					Get_NumChannels()	{ return m_iNumChannels; }
	vector<class CChannel*>	Get_VecChannels()	{ return m_vecChannels; }
	_bool					Get_CanStop()		{ return m_CanStop; }
	_bool					Get_Loop()			{ return m_Loop; }
	_bool					Get_Finished()		{ return m_bFinished; }


private:

	_char					m_szName[MAX_PATH] = "";
	_double					m_Duration = { 0.0 };		/* �� �ִϸ��̼��� ��ü ����. */
	_double					m_TickPerSecond = { 0.0 };	/* �ʴ� ��� �ӵ� .*/
	_double					m_TrackPosition = { 0.0 };	/* ���� �� �ִϸ��̼��� ��� ��ġ */


	_uint					m_iNumChannels = { 0 };		/* �� �ִϸ��̼��� �����ϴµ� �ʿ��� ���� ����. */
	vector<class CChannel*>	m_vecChannels = {};			/* �� �ִϸ��̼��� �����ϴµ� �ʿ��� ���� */

	vector<_uint>			m_iCurrentKeyFrames;

	_bool					m_bFinished = { false };		// �ִϸ��̼��� ��������
	_bool					m_CanStop	= { false };		// �ִϸ��̼��� ��� �Ǵ���
	_bool					m_Loop		= { true };			// ������ ������


public:
	static CAnimation* Create();
	CAnimation* Clone();
	virtual void Free() override;

};

END