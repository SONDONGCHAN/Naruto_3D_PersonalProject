#pragma once
#include "Client_Defines.h"
#include "Camera.h"


BEGIN(Client)

class CCamera_Free final : public CCamera
{
public:
	enum CAMERA_STATE { CAMERA_FREE, CAMERA_PLAYER_CHASE, CAMERA_TARGET_CHASE, CAMERA_DEBUG, CAMERA_END };
	enum CAMERA_POINT_VIEW { PLAYER_FRONT, PLAYER_BACK, PLAYER_RIGHT, PLAYER_LEFT , CAMERA_POINT_END };
	enum CAMERA_SHAKE_DIR{ SHAKE_ALL, SHAKE_HORIZONTAL, SHAKE_VERTICAL, SHAKE_END };

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
	_float	Float_Lerp(_float start, _float end, _float ratio);

public:
	CAMERA_STATE	Get_Camera_State() { return m_eCamera_State; }
	void	Set_Target_Position(_vector* _Target_Pos) { m_pTarget_Pos = _Target_Pos; }

	// ī�޶� �ɼ� ���� ::
	 
	// CAMERA_PLAYER_CHASE : ���� ���, CAMERA_FREE : Ŀ���� ���, CAMERA_DEBUG : ����� ���
	void	Set_Camera_State(CAMERA_STATE _Option) { m_eCamera_State = _Option; }

	// ī�޶� ���� Ư�� ��ǥ�� ���� & �ٶ���ǥ ( false: ���� ��� ����, true: ���� �ε巴�� �̵�)
	void	Set_Camera_Pos_Lerf(_float4 TargetEye, _float4 TargetAt, _bool _bSoft, _float DurTime = 0.f);

	void	Set_Camera_Point(_matrix* UserMatrix, CAMERA_POINT_VIEW _ViewPoint);

	// ī�޶� ����ŷ (����ŷ ����, ����ŷ �ð�)
	void	ShakeCamera(CAMERA_SHAKE_DIR Shake_Option, _float _ShakingSize, _float _ShakingTime);

	// ī�޶� �Ÿ� ����
	void	Set_Camera_radius(_float radius = 4.f, _float LerpRatio = 0.2f)  {
		m_Target_radius = radius; 
		m_fLerpRatio = LerpRatio;
	}
	void	Set_Camera_radius_Immediate(_float radius ) { m_radius = radius; }

private:
		
	_float			m_fMouseSensor = { 0.f }; // ���콺 ����

	// CAMERA_PLAYER_CHASE
	_float			m_Target_radius = { 4.f };		// ���� ��ǥ ������ (Target)
	_float			m_radius = { 4.f };				// ���� ��ǥ ������
	_float			m_degree_Horizontal = { 0.f };	// ���� ��ǥ ���� ����
	_float			m_degree_Vertical = { 0.f };	// ���� ��ǥ ���� ����
	_float			Column_Max = { 85.f };			// ���� ���� �ִ�
	_float			Column_Min = { -10.f };			// ���� ���� �ּ�

	//CAMERA_TARGET_CHASE
	_vector*			m_pTarget_Pos ;

	// ī�޶� ����ŷ ����
	_float	m_fShakingSize = 0.0f;
	_float	m_fShakingTime = 0.0f;
	_float	m_fShakeTimer = 0.0f;
	_float4	m_vCameraOldPos;
	_bool	m_bShaking = false;
	CAMERA_SHAKE_DIR m_eShaking_Option = { SHAKE_ALL };
	random_device		m_RandomDevice;
	mt19937_64			m_RandomNumber;

	// ī�޶� ����
	CAMERA_STATE	m_eCamera_State = CAMERA_PLAYER_CHASE;

	// ī�޶� ���� ����
	_float m_fLerpRatio = 0.2f;

	// ī�޶� �������� ����
	CAMERA_POINT_VIEW	m_ePointWiew = { PLAYER_FRONT };
	_matrix*			m_pUserMatrix = { nullptr };
	
	LEVEL		m_Current_LEVEL;


private:
	HRESULT Add_Component();

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END