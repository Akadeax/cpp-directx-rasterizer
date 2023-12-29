#include "pch.h"
#include "Effect_DiffusePartialCoverage.h"

Effect_DiffusePartialCoverage::Effect_DiffusePartialCoverage(ID3D11Device* pDevice, Texture* pDiffuse)
	: Effect{ pDevice, L"Resources/DiffusePartialCoverage.fx", pDiffuse }
{
}
