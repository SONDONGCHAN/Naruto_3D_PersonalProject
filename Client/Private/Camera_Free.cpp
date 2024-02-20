#include "stdafx.h"
#include "Camera_Free.h"
#include "Player.h"


CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCamera(pDevice, pContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& rhs)
	:CCamera(rhs)
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
	CAMERA_FREE_DESC* pCameraFreeDesc = (CAMERA_FREE_DESC*)pArg;

	m_fMouseSensor = pCameraFreeDesc->fMouseSensor;
	m_Current_LEVEL = pCameraFreeDesc->Current_LEVEL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_RandomNumber = mt19937_64(m_RandomDevice());

	return S_OK;
}

void CCamera_Free::Priority_Tick(_float fTimeDelta)
{
	Key_Input(fTimeDelta);
}

void CCamera_Free::Tick(_float fTimeDelta)
{
	UpdateCameraShake(fTimeDelta);
}

void CCamera_Free::Late_Tick(_float fTimeDelta)
{
	if (m_Current_LEVEL == LEVEL_GAMEPLAY)
	{
		m_radius = Float_Lerp(m_radius, m_Target_radius, m_fLerpRatio);


		if (m_eCamera_State == CAMERA_FREE)
		{
			if (m_ePointWiew == PLAYER_FRONT)
			{
				_vector UserPos = (*m_pUserMatrix).r[3];
				_vector Look = (*m_pUserMatrix).r[2];
				_vector Pos;

				UserPos.m128_f32[1] += 1.5f;
				Look = XMVector3Normalize(Look);

				Pos = UserPos + Look * m_radius;
				m_pTransformCom->Set_Pos(Pos);
				m_pTransformCom->LookAt(UserPos);
			}
			if (m_ePointWiew == PLAYER_BACK)
			{
				_vector UserPos = (*m_pUserMatrix).r[3];
				_vector Look = (*m_pUserMatrix).r[2];
				_vector Pos;

				UserPos.m128_f32[1] += 1.5f;
				Look = XMVector3Normalize(Look);

				Pos = UserPos - Look * m_radius;
				m_pTransformCom->Set_Pos(Pos);
				m_pTransformCom->LookAt(UserPos);
			}
		}

		// @@현재는 플레이어를 추적 하도록 되어있음@@
		if (m_eCamera_State == CAMERA_PLAYER_CHASE)
		{
			// 구면 좌표계를 이용하여 카메라의 위치를 잡아주자
			_float3 camPos = Cal_Pos(m_radius, m_degree_Horizontal, m_degree_Vertical);

			// 플레이어를 중심으로 회전 할 수 있도록 플레이어 위치를 받아오자
			CTransform* pPlayerTransform = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")))->Get_CurrentCharacter()->Get_TranformCom();
			_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_float4 revisePos;
			XMStoreFloat4(&revisePos, vPlayerPos);

			// 플레이어의 위치보다 살짝 높은곳을 기준으로 잡자
			revisePos.y += 1.5f;

			// 플레이어의 위치를 더해줘서 최종 카메라의 위치를 잡아주자
			_vector TargetPos = XMLoadFloat3(&camPos) + XMLoadFloat4(&revisePos);

			// 카메라의 현재 위치와 목표위치를 선형보간하여 최종 위치를 잡아주자.
			_vector CurrentCamPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector Lerp_Pos = XMVectorLerp(CurrentCamPos, TargetPos, 0.2f);

			//  카메라의 현재 방향와 목표방향를 선형보간하여 최종 방향을 잡아주자.
			_vector CamDir = XMVector3Normalize( XMLoadFloat4(&revisePos) - CurrentCamPos );
			_vector CurrentCamDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			_vector Lerp_Dir = XMVectorLerp(CurrentCamDir, CamDir, 0.2f);

			//  Eye와 At을 설정해주자
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, Lerp_Pos);
			m_pTransformCom->Set_Look_IncludeY(Lerp_Dir);

			//m_pTransformCom->LookAt(XMLoadFloat4(&revisePos));

			////////////////////////////////////////////////////////////////////////////////////////////////////////////
		}


		if (m_eCamera_State == CAMERA_TARGET_CHASE)
		{
		}

	}


	if (FAILED(SetUp_TransformMatices()))
		return;
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

void CCamera_Free::Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_C))
	{
		if (m_eCamera_State == CAMERA_DEBUG)
			m_eCamera_State = CAMERA_PLAYER_CHASE;
		else
			m_eCamera_State = CAMERA_DEBUG;
	}

	if (m_eCamera_State == CAMERA_PLAYER_CHASE)
	{
		_long	MouseMove = { 0 };
		
		if (MouseMove = m_pGameInstance->Mouse_Move(DIMS_X))
		{
			m_degree_Horizontal -= m_fMouseSensor* MouseMove;
		}
		
		if (MouseMove = m_pGameInstance->Mouse_Move(DIMS_Y))
		{
			m_degree_Vertical += m_fMouseSensor * MouseMove;

			if (m_degree_Vertical > Column_Max)
				m_degree_Vertical = Column_Max;

			else if (m_degree_Vertical < Column_Min)
				m_degree_Vertical = Column_Min;
		}
	}

	if (m_eCamera_State == CAMERA_DEBUG)
	{
		if (m_pGameInstance->Key_Pressing(DIK_UP, fTimeDelta))
		{
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
		if (m_pGameInstance->Key_Pressing(DIK_DOWN, fTimeDelta))
		{
			m_pTransformCom->Go_Backward(fTimeDelta);
		}
		if (m_pGameInstance->Key_Pressing(DIK_LEFT, fTimeDelta))
		{
			m_pTransformCom->Go_Left(fTimeDelta);
		}
		if (m_pGameInstance->Key_Pressing(DIK_RIGHT, fTimeDelta))
		{
			m_pTransformCom->Go_Right(fTimeDelta);
		}

		_long	MouseMove = { 0 };

		if (MouseMove = m_pGameInstance->Mouse_Move(DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
		}

		if (MouseMove = m_pGameInstance->Mouse_Move(DIMS_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
		}
	}
}

_float3 CCamera_Free::Cal_Pos(_float radius, _float Horizontal_degree, _float column_degree)
{
	_float3 CameraPos;

	CameraPos.x = radius * cos(XMConvertToRadians(column_degree)) * cos(XMConvertToRadians(Horizontal_degree));
	CameraPos.y = radius * sin(XMConvertToRadians(column_degree));
	CameraPos.z = radius * cos(XMConvertToRadians(column_degree)) * sin(XMConvertToRadians(Horizontal_degree));
	
	return CameraPos;
}

void CCamera_Free::UpdateCameraShake(const _float& fTimeDelta)
{
	if (m_bShaking)
	{
		if (m_fShakeTimer < m_fShakingTime)
		{
			if (m_eShaking_Option == SHAKE_ALL)
			{
				uniform_real_distribution <float>	ShakeRange(-0.5f, 0.5f);

				_float fShake_Scale = ShakeRange(m_RandomNumber);

				if (fShake_Scale > 0)
					fShake_Scale += 0.5f;
				else
					fShake_Scale -= 0.5f;

				m_fFovy = XMConvertToRadians(60.f + fShake_Scale* m_fShakingSize);
			}

			m_fShakeTimer += fTimeDelta;
		}
		else
		{
			m_bShaking = false;
			m_fFovy = XMConvertToRadians(60.f);
		}
		
	}
}

_float CCamera_Free::Float_Lerp(_float start, _float end, _float ratio)
{
	return start + ratio * (end - start);
}

void CCamera_Free::Set_Camera_Pos_Lerf(_float4 TargetEye, _float4 TargetAt, _bool _bSoft, _float DurTime)
{
	//  Eye와 At을 설정해주자
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, Lerf_Pos);
	//m_pTransformCom->LookAt(XMLoadFloat4(&revisePos));
}

void CCamera_Free::Set_Camera_Point(_matrix* UserMatrix, CAMERA_POINT_VIEW _ViewPoint)
{
	m_pUserMatrix = UserMatrix;
	m_ePointWiew = _ViewPoint;

	 if (m_ePointWiew == PLAYER_FRONT)
	 {
	 	_vector UserPos = (*m_pUserMatrix).r[3];
	 	_vector Look = (*m_pUserMatrix).r[2];
	 	_vector Pos;
	 
	 	UserPos.m128_f32[1] += 1.5f;
	 	Look = XMVector3Normalize(Look);
	 
	 	Pos = UserPos + Look * m_radius;
	 	m_pTransformCom->Set_Pos(Pos);
	 	m_pTransformCom->LookAt(UserPos);
	 }
	 if (m_ePointWiew == PLAYER_BACK)
	 {
	 	_vector UserPos = (*m_pUserMatrix).r[3];
	 	_vector Look = (*m_pUserMatrix).r[2];
	 	_vector Pos;
	 
	 	UserPos.m128_f32[1] += 1.5f;
	 	Look = XMVector3Normalize(Look);
	 
	 	Pos = UserPos - Look * m_radius;
	 	m_pTransformCom->Set_Pos(Pos);
	 	m_pTransformCom->LookAt(UserPos);
	 }
}

void CCamera_Free::ShakeCamera(CAMERA_SHAKE_DIR Shake_Option, _float _ShakingSize, _float _ShakingTime)
{
	if (!m_bShaking)
	{
		m_bShaking = true;
		m_fShakingSize = _ShakingSize;
		m_fShakingTime = _ShakingTime;
		m_fShakeTimer = 0.0f;
		m_eShaking_Option = Shake_Option;
	}
}

HRESULT CCamera_Free::Add_Component()
{
	return S_OK;
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Free");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Free");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();
}
