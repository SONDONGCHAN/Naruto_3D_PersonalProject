#include "VIBuffer_Instancing.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs)
	: CVIBuffer(rhs)
	//, m_pVBInstance(rhs.m_pVBInstance)
	//, m_iInstanceStride(rhs.m_iInstanceStride)
	//, m_iNumInstance(rhs.m_iNumInstance)
	//, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
	, m_RandomNumber(rhs.m_RandomNumber)
	//, m_pSpeed(rhs.m_pSpeed)
	//, m_pLifeTime(rhs.m_pLifeTime)
	//, m_pMaxLifeTime(rhs.m_pMaxLifeTime)
	//, m_pMaxSizeY(rhs.m_pMaxSizeY)
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Instancing::Initialize_Prototype()
{
	m_RandomNumber = mt19937_64(m_RandomDevice());

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(void* pArg)
{
	m_InstanceData = *(INSTANCE_DESC*)pArg;

	m_iNumInstance = m_InstanceData.iNumInstance;

	m_pSpeed	= new _float[m_iNumInstance];
	m_pLifeTime = new _float[m_iNumInstance];
	m_pMaxLifeTime = new _float[m_iNumInstance];
	m_pMaxSizeY = new _float[m_iNumInstance];

	uniform_real_distribution<float>	SpeedRange(m_InstanceData.vSpeed.x, m_InstanceData.vSpeed.y);
	uniform_real_distribution<float>	TimeRange(m_InstanceData.vLifeTime.x, m_InstanceData.vLifeTime.y);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		m_pSpeed[i]		= SpeedRange(m_RandomNumber);
		m_pLifeTime[i]	= TimeRange(m_RandomNumber);
		m_pMaxLifeTime[i] = TimeRange(m_RandomNumber);

	}

	m_isFinished = { true };
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Bind_Buffers()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint				iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride
	};

	_uint				iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

_float CVIBuffer_Instancing::Lerp(_float start, _float end, _float ratio)
{
	return start + ratio * (end - start);
}

_bool CVIBuffer_Instancing::Trigger(_vector vCenterPos)
{
	if (m_isFinished == true)
	{
		m_isFinished = false;
		m_fTimeAcc = 0.f;
		XMStoreFloat3(&m_InstanceData.vCenter, vCenterPos);
		m_InstanceData.vPivot = m_InstanceData.vCenter;

		uniform_real_distribution<float>	TimeRange(m_InstanceData.vLifeTime.x, m_InstanceData.vLifeTime.y);
		uniform_real_distribution<float>	WidthRange(m_InstanceData.vRange.x * -0.5f, m_InstanceData.vRange.x * 0.5f);
		uniform_real_distribution<float>	HeightRange(m_InstanceData.vRange.y * -0.5f, m_InstanceData.vRange.y * 0.5f);
		uniform_real_distribution<float>	DepthRange(m_InstanceData.vRange.z * -0.5f, m_InstanceData.vRange.z * 0.5f);
		uniform_real_distribution<float>	SizeRange(m_InstanceData.vSize.x, m_InstanceData.vSize.y);


		m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &m_SubResource);

		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			m_pLifeTime[i] = TimeRange(m_RandomNumber);
			m_pMaxLifeTime[i] = m_pLifeTime[i];

			((VTXINSTANCE*)m_SubResource.pData)[i].vTranslation = _float4(
				m_InstanceData.vCenter.x + WidthRange(m_RandomNumber),
				m_InstanceData.vCenter.y + HeightRange(m_RandomNumber),
				m_InstanceData.vCenter.z + DepthRange(m_RandomNumber),
				1.f);

			((VTXINSTANCE*)m_SubResource.pData)[i].vColor.w = 1.f;

			_float		fSize = SizeRange(m_RandomNumber);

			((VTXINSTANCE*)m_SubResource.pData)[i].vRight = _float4(fSize, 0.f, 0.f, 0.f);
			((VTXINSTANCE*)m_SubResource.pData)[i].vUp = _float4(0.f, fSize * 60.f, 0.f, 0.f);
			m_pMaxSizeY[i] = fSize * 60.f;
			((VTXINSTANCE*)m_SubResource.pData)[i].vLook = _float4(0.f, 0.f, fSize, 0.f);
			

			_vector		vDir = XMLoadFloat4(&((VTXINSTANCE*)m_SubResource.pData)[i].vTranslation) - XMLoadFloat3(&m_InstanceData.vPivot);
			vDir = XMVectorSetW(vDir, 0.f);
			XMStoreFloat4(&((VTXINSTANCE*)m_SubResource.pData)[i].vTranslation, XMLoadFloat4(&((VTXINSTANCE*)m_SubResource.pData)[i].vTranslation) + XMVector3Normalize(vDir) * fSize * 60.f );
			//XMStoreFloat4(&((VTXINSTANCE*)m_SubResource.pData)[i].vTranslation, XMLoadFloat4(&((VTXINSTANCE*)m_SubResource.pData)[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeed[i] * 0.016);

			//_vector		vDir = XMLoadFloat4(&((VTXINSTANCE*)m_SubResource.pData)[i].vTranslation) - XMLoadFloat3(&m_InstanceData.vPivot);
			//vDir = XMVectorSetW(vDir, 0.f);
			//vDir = XMVector3Normalize(vDir);
			//XMStoreFloat4(&((VTXINSTANCE*)m_SubResource.pData)[i].vTranslation, (XMLoadFloat3(&m_InstanceData.vPivot) + (vDir * fSize * 60.f) ) );
		}

		return true;
	}

	else
		return false;
}

void CVIBuffer_Instancing::Tick_Particle(_float fTimeDelta)
{
	if (m_InstanceData.MyOption == OPTION_DROP)
	{
		if(m_InstanceData.isLoop == true)
			Tick_Drop(fTimeDelta);
		else if (m_InstanceData.isLoop == false)
			Tick_Drop(fTimeDelta);
	}
	else if (m_InstanceData.MyOption == OPTION_SPREAD)
	{
		if (m_InstanceData.isLoop == true)
			Tick_Spread_Loop(fTimeDelta);
		else if (m_InstanceData.isLoop == false)
			Tick_Spread(fTimeDelta);
	}
}

void CVIBuffer_Instancing::Tick_Drop(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_InstanceData.fDuration <= m_fTimeAcc)
		m_isFinished = true;

	D3D11_MAPPED_SUBRESOURCE			SubResource{};
	
	uniform_real_distribution<float>	TimeRange(m_InstanceData.vLifeTime.x, m_InstanceData.vLifeTime.y);
	uniform_real_distribution<float>	WidthRange(m_InstanceData.vRange.x * -0.5f, m_InstanceData.vRange.x * 0.5f);
	uniform_real_distribution<float>	HeightRange(m_InstanceData.vRange.y * -0.5f, m_InstanceData.vRange.y * 0.5f);
	uniform_real_distribution<float>	DepthRange(m_InstanceData.vRange.z * -0.5f, m_InstanceData.vRange.z * 0.5f);


	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXINSTANCE*)SubResource.pData)[i].vTranslation.y -= m_pSpeed[i] * fTimeDelta;

		m_pLifeTime[i] -= fTimeDelta;

		if (0.0f >= m_pLifeTime[i])
		{
			if (true == m_InstanceData.isLoop)
			{
				m_pLifeTime[i] = TimeRange(m_RandomNumber);
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation = _float4(
					m_InstanceData.vCenter.x + WidthRange(m_RandomNumber),
					m_InstanceData.vCenter.y + HeightRange(m_RandomNumber),
					m_InstanceData.vCenter.z + DepthRange(m_RandomNumber),
					1.f);
			}
			else
			{
				((VTXINSTANCE*)SubResource.pData)[i].vColor.w = 0.f;
			}
		}

	
	}
	
	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instancing::Tick_Spread_Loop(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_InstanceData.fDuration <= m_fTimeAcc)
		m_isFinished = true;

	D3D11_MAPPED_SUBRESOURCE			SubResource{};

	uniform_real_distribution<float>	TimeRange(m_InstanceData.vLifeTime.x, m_InstanceData.vLifeTime.y);
	uniform_real_distribution<float>	WidthRange(m_InstanceData.vRange.x * -0.5f, m_InstanceData.vRange.x * 0.5f);
	uniform_real_distribution<float>	HeightRange(m_InstanceData.vRange.y * -0.5f, m_InstanceData.vRange.y * 0.5f);
	uniform_real_distribution<float>	DepthRange(m_InstanceData.vRange.z * -0.5f, m_InstanceData.vRange.z * 0.5f);

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		VTXINSTANCE* pVertices = ((VTXINSTANCE*)SubResource.pData);

		_vector		vDir = XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceData.vPivot);
		vDir = XMVectorSetW(vDir, 0.f);

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeed[i] * fTimeDelta);

		m_pLifeTime[i] -= fTimeDelta;

		if (0.0f >= m_pLifeTime[i])
		{
			((VTXINSTANCE*)SubResource.pData)[i].vColor.w = 0.f;

			if ((true == m_InstanceData.isLoop) && (m_isFinished))
			{
				((VTXINSTANCE*)SubResource.pData)[i].vColor.w = 1.f;

				m_pLifeTime[i] = TimeRange(m_RandomNumber);
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation = _float4(
					m_InstanceData.vCenter.x + WidthRange(m_RandomNumber),
					m_InstanceData.vCenter.y + HeightRange(m_RandomNumber),
					m_InstanceData.vCenter.z + DepthRange(m_RandomNumber),
					1.f);

				if (m_iNumInstance <= i + 1)
				{
					m_isFinished = false;
					m_fTimeAcc = 0.f;
				}
			}
		}
	}
	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instancing::Tick_Spread(_float fTimeDelta)
{
	if (!m_isFinished)
	{
		m_fTimeAcc += fTimeDelta;

		if (m_InstanceData.fDuration <= m_fTimeAcc)
			m_isFinished = true;

		m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &m_SubResource);

		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			VTXINSTANCE* pVertices = ((VTXINSTANCE*)m_SubResource.pData);

			_vector		vDir = XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceData.vPivot);
			vDir = XMVectorSetW(vDir, 0.f);

			XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeed[i] * fTimeDelta);

			m_pLifeTime[i] -= fTimeDelta;

			if (0.0f >= m_pLifeTime[i])
			{
				((VTXINSTANCE*)m_SubResource.pData)[i].vColor.w = 0.f;
			}
			else
			{
				_float Alpha = m_pLifeTime[i] / m_pMaxLifeTime[i];
				if (Alpha < 0)
					Alpha = 0.f;
				((VTXINSTANCE*)m_SubResource.pData)[i].vColor.w = Alpha;

				m_pMaxSizeY[i] = Lerp(m_pMaxSizeY[i], 0.f, 0.15f );
				((VTXINSTANCE*)m_SubResource.pData)[i].vUp = _float4(0.f, m_pMaxSizeY[i], 0.f, 0.f);
			}
		}
		m_pContext->Unmap(m_pVBInstance, 0);
	}
}


void CVIBuffer_Instancing::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pMaxSizeY);
	Safe_Delete_Array(m_pMaxLifeTime);
	Safe_Delete_Array(m_pLifeTime);
	Safe_Delete_Array(m_pSpeed);

	Safe_Release(m_pVBInstance);
}
