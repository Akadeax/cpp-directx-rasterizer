#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Vector3 origin{};
		float verticalFovAngle{ 90.f };
		float fov{ tanf((verticalFovAngle * TO_RADIANS) / 2.f) };
		float aspectRatio{ 1.66f };

		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{};
		float totalYaw{};

		Matrix invViewMatrix{};
		Matrix viewMatrix{};
		Matrix projectionMatrix{};

		float cameraRotationSpeed{ 0.04f };
		float cameraTranslationSpeed{ 25.f };
		float cameraTranslationSpeedBoosted{ 50.f };

		float nearPlane{ 1.f };
		float farPlane{ 1000.f };

		bool boost{ false };

		void Initialize(float _verticalFovAngle = 90.f, Vector3 _origin = {0.f,0.f,0.f}, float _aspectRatio = 1.66f)
		{
			verticalFovAngle = _verticalFovAngle;
			fov = tanf((verticalFovAngle * TO_RADIANS) / 2.f);

			origin = _origin;

			aspectRatio = _aspectRatio;

			CalculateProjectionMatrix();
		}

		// World to Camera transformation
		void CalculateViewMatrix()
		{
			const Matrix rotation{ Matrix::CreateRotation(totalPitch, totalYaw, 0.f) };

			forward = rotation.TransformVector(Vector3::UnitZ);
			right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
			up = Vector3::Cross(forward, right).Normalized();

			invViewMatrix = Matrix(right, up, forward, origin);
			viewMatrix = invViewMatrix.Inverse();
		}

		void CalculateProjectionMatrix()
		{
			projectionMatrix = {
				Vector4{ 1.f / (aspectRatio * fov), 0,         0,                                                0 },
				Vector4{ 0,                         1.f / fov, 0,                                                0 },
				Vector4{ 0,                         0,         farPlane / (farPlane - nearPlane),                1 },
				Vector4{ 0,                         0,         -(farPlane * nearPlane) / (farPlane - nearPlane), 0 },
			};
		}

		void Update(const Timer* pTimer)
		{
			const float deltaTime{ pTimer->GetElapsed() };

			// Keyboard Input
			const uint8_t* pKeyboardState{ SDL_GetKeyboardState(nullptr) };

			// Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState{ SDL_GetRelativeMouseState(&mouseX, &mouseY) };

			const bool leftMouse{ (mouseState & SDL_BUTTON(1)) != 0 };
			const bool rightMouse{ (mouseState & SDL_BUTTON(3)) != 0 };

			const float tSpeed{ boost ? cameraTranslationSpeedBoosted : cameraTranslationSpeed };

			// == Translation ==
			// WASD
			if (pKeyboardState[SDL_SCANCODE_W])
			{
				origin += forward * tSpeed * deltaTime;
			}
			else if (pKeyboardState[SDL_SCANCODE_S])
			{
				origin -= forward * tSpeed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_D])
			{
				origin += right * tSpeed * deltaTime;
			}
			else if (pKeyboardState[SDL_SCANCODE_A])
			{
				origin -= right * tSpeed * deltaTime;
			}

			// LMB + MouseY
			if (leftMouse && !rightMouse && mouseY != 0)
			{
				origin -= forward * static_cast<float>(mouseY) * tSpeed * 100 * deltaTime;
			}
			// LMB + RMB + MouseY
			if (leftMouse && rightMouse && mouseY != 0)
			{
				origin += Vector3::UnitY * static_cast<float>(mouseY) * tSpeed * 100 * deltaTime;
			}

			// Rotation
			if (leftMouse && !rightMouse && mouseX != 0)
			{
				totalYaw += cameraRotationSpeed * static_cast<float>(mouseX);
			}
			else if (!leftMouse && rightMouse)
			{
				totalYaw += cameraRotationSpeed * static_cast<float>(mouseX);
				totalPitch -= cameraRotationSpeed * static_cast<float>(mouseY);
			}

			if (pKeyboardState[SDL_SCANCODE_SPACE])
			{
				totalYaw += cameraRotationSpeed * deltaTime;
			}

			//Update Matrices
			CalculateViewMatrix();
		}
	};
}
