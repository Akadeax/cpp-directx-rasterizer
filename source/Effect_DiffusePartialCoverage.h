#pragma once
#include "Effect.h"

class Effect_DiffusePartialCoverage final : public Effect
{
public:
	explicit Effect_DiffusePartialCoverage(
		ID3D11Device* pDevice,
		Texture* pDiffuse
	);
	~Effect_DiffusePartialCoverage() override = default;

	Effect_DiffusePartialCoverage(const Effect_DiffusePartialCoverage&) = delete;
	Effect_DiffusePartialCoverage(Effect_DiffusePartialCoverage&&) noexcept = delete;
	Effect_DiffusePartialCoverage& operator=(const Effect_DiffusePartialCoverage&) = delete;
	Effect_DiffusePartialCoverage& operator=(Effect_DiffusePartialCoverage&&) noexcept = delete;
};

