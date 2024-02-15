#pragma once

#include "Base.h"


BEGIN(Engine)

class CTimer_Manager final : public CBase
{
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	_float Compute_TimeDelta(const wstring& strTimerTag);

public:
	HRESULT Add_Timer(const wstring& strTimerTag);

private:
	class CTimer* Find_Timer(const wstring& strTimerTag);

private:
	map<const wstring, class CTimer*>		m_Timers;

public:
	static CTimer_Manager* Create();
	virtual void Free(void);
};

END