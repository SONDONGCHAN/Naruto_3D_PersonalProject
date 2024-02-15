#pragma once
#include "Client_Defines.h"
#include "Camera.h"


BEGIN(Client)

class CCamera_Free final : public CCamera
{
public:
	enum CAMERA_STATE { CAMERA_FREE, CAMERA_CHASE, CAMERA_DEBUG, CAMERA_END };

public:
	struct CAMERA_FREE_DESC : public CCamera::CAMERA_DESC
	{
		_float fMouseSensor = { 0.f };
		LEVEL Current_LEVEL;
	};

private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& rhs);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void	Key_Input(_float fTimeDelta);
	_float3	Cal_Pos(_float radius, _float Horizontal_degree, _float column_degree);
	void	UpdateCameraShake(const _float& fTimeDelta); // ����ŷ ������Ʈ


public:
	CAMERA_STATE	Get_Camera_State() { return m_eCamera_State; }

	// ī�޶� �ɼ� ���� ::
	 
	// CAMERA_CHASE : ���� ���, CAMERA_FREE : Ŀ���� ���, CAMERA_DEBUG : ����� ���
	void	Set_Camera_State(CAMERA_STATE _Option) { m_eCamera_State = _Option; }

	// ī�޶� ���� Ư�� ��ǥ�� ���� & �ٶ���ǥ ( false: ���� ��� ����, true: ���� �ε巴�� �̵�)
	void	Set_Camera_Pos_Lerf(_float4 TargetEye, _float4 TargetAt, _bool _bSoft, _float DurTime = 0.f);

	// ī�޶� ����ŷ (����ŷ ����, ����ŷ �ð�)
	void	ShakeCamera(_float _ShakingSize, _float _ShakingTime);

private:
		
	_float			m_fMouseSensor = { 0.f }; // ���콺 ����

	// Chase��� ����
	_float			m_radius = { 4.f };				// ���� ��ǥ ������
	_float			m_degree_Horizontal = { 0.f };	// ���� ��ǥ ���� ����
	_float			m_degree_Column = { 0.f };		// ���� ��ǥ ���� ����
	_float			Column_Max = { 85.f };			// ���� ���� �ִ�
	_float			Column_Min = { -10.f };			// ���� ���� �ּ�

	// ī�޶� ����ŷ ����
	_float	m_fShakingSize = 0.0f;
	_float	m_fShakingTime = 0.0f;
	_float	m_fShakeTimer = 0.0f;
	_float4	m_vCameraOldPos;
	_bool	m_bShaking = false;

	CAMERA_STATE	m_eCamera_State = CAMERA_CHASE;

	//
	_float m_fX;
	_float m_fY;

	LEVEL		m_Current_LEVEL;


private:
	HRESULT Add_Component();

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END