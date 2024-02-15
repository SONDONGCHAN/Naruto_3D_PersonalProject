#include "..\Public\Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTexture::CTexture(const CTexture& rhs)
	: CComponent(rhs)
	, m_SRVs(rhs.m_SRVs)
	, m_iNumTextures(rhs.m_iNumTextures)
{
	for (auto& pSRV : m_SRVs)
		Safe_AddRef(pSRV);
}

HRESULT CTexture::Initialize_Prototype(const wstring& strTextureFilePath, _uint iNumTextures)
{
	m_iNumTextures = iNumTextures;

	_tchar		szEXT[MAX_PATH] = TEXT("");

	_wsplitpath_s(strTextureFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);


	for (size_t i = 0; i < iNumTextures; i++)
	{
		_tchar		szTextureFilePath[MAX_PATH] = TEXT("");

		wsprintf(szTextureFilePath, strTextureFilePath.c_str(), i);

		HRESULT			hr = { 0 };

		ID3D11ShaderResourceView* pSRV = { nullptr };

		// CreateDDSTextureFromFile 는 
		// ID3D11ShaderResourceView* 타입과 ID3D11Resource* 타입을 둘다 반환 가능.
		// 여기서는 ID3D11ShaderResourceView* 만 반환.

		if (false == lstrcmp(szEXT, TEXT(".dds")))
			hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

		else if (false == lstrcmp(szEXT, TEXT(".tga")))
		{
			hr = E_FAIL;
			MSG_BOX("Failed to TextureLoad : Texture is .tga");
		}

		else
			hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

		if (FAILED(hr))
			return E_FAIL;

		m_SRVs.push_back(pSRV);
	}

	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iIndex)
{
	if (iIndex >= m_iNumTextures)
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_SRVs[iIndex]);
}

HRESULT CTexture::Bind_ShaderResources(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_SRVs(pConstantName, &m_SRVs.front(), m_iNumTextures);
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_SRVs)
		Safe_Release(pSRV);

	m_SRVs.clear();
}