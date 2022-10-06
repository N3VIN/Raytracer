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
		// Data Types
		Vector3 origin{};
		float fovAngle{};
		float degrees{};

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };
		
		// manually trying to rotate doesnt work properly as showed in the pdf.
		/*Vector3 forward{ 0.266f, -0.453f, 0.860f };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };*/

		float totalPitch{ 0.f };
		float totalYaw{ 0.f };

		Matrix cameraToWorld{};

		// Constructor & Destructor
		Camera() = default;
		Camera(const Vector3& _origin, float _fovAngle) :
			origin{ _origin },
			degrees{_fovAngle}
		{
			fovAngle = tan((TO_RADIANS * degrees) / 2);
		}

		~Camera() = default;

		Camera(const Camera&) = default;
		Camera(Camera&&) noexcept = default;
		Camera& operator=(const Camera&) = default;
		Camera& operator=(Camera&&) noexcept = default;
		


		Matrix CalculateCameraToWorld()
		{
			right = Vector3::Cross(up, forward).Normalized();
			up = Vector3::Cross(forward, right).Normalized();

			Matrix ONB{};

			// right
			ONB[0][0] = right.x;
			ONB[0][1] = right.y;
			ONB[0][2] = right.z;
			ONB[0][3] = 0;
			// up
			ONB[1][0] = up.x;
			ONB[1][1] = up.y;
			ONB[1][2] = up.z;
			ONB[1][3] = 0;
			// forward
			ONB[2][0] = forward.x;
			ONB[2][1] = forward.y;
			ONB[2][2] = forward.z;
			ONB[2][3] = 0;
			// position
			ONB[3][0] = origin.x;
			ONB[3][1] = origin.y;
			ONB[3][2] = origin.z;
			ONB[3][3] = 1;

			return ONB;
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();




			//Keyboard Input
			//const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
			KeyboardMovement(deltaTime);

			//Mouse Input
			//int mouseX{}, mouseY{};
			//const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			MouseMovement(deltaTime);
		}

		void updateFovAngle(float angle)
		{
			degrees = angle;
			fovAngle = tan((TO_RADIANS * degrees) / 2);

		}

		void KeyboardMovement(float deltaTime)
		{
			float velocity{ 5.f };
			const Uint8* pKeyboardState = SDL_GetKeyboardState(NULL);
			if (pKeyboardState[SDL_SCANCODE_W])
			{
				origin += forward * velocity * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_UP])
			{
				origin += forward * velocity * deltaTime;
			}

			if (pKeyboardState[SDL_SCANCODE_S])
			{
				origin -= forward * velocity * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_DOWN])
			{
				origin -= forward * velocity * deltaTime;
			}

			if (pKeyboardState[SDL_SCANCODE_A])
			{
				origin -= right * velocity * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_LEFT])
			{
				origin -= right * velocity * deltaTime;
			}

			if (pKeyboardState[SDL_SCANCODE_D])
			{
				origin += right * velocity * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_RIGHT])
			{
				origin += right * velocity * deltaTime;
			}

			// unreal style up and down.
			if (pKeyboardState[SDL_SCANCODE_C])
			{
				origin -= up * velocity * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_Z])
			{
				origin += up * velocity * deltaTime;
			}

			// fov change.
			if (pKeyboardState[SDL_SCANCODE_KP_PLUS])
			{
				degrees++;
				updateFovAngle(degrees);
			}
			if (pKeyboardState[SDL_SCANCODE_KP_MINUS])
			{
				degrees--;
				updateFovAngle(degrees);

			}
		}

		void MouseMovement(float deltaTime)
		{

			int mouseX, mouseY;
			float sensitivity{ 1.f };
			auto mouse = SDL_GetRelativeMouseState(&mouseX, &mouseY);


			if (mouse == SDL_BUTTON(3))
			{
				totalYaw += mouseX * (sensitivity * deltaTime);
				totalPitch += mouseY * (sensitivity * deltaTime);
			}
			else if (mouse == SDL_BUTTON(1))
			{
				origin += forward * (mouseY * (sensitivity * deltaTime));
				totalYaw += mouseX * (sensitivity * deltaTime);
			}

			Matrix rotationMatrix = Matrix::CreateRotationX(totalPitch) * Matrix::CreateRotationY(totalYaw);
			forward = rotationMatrix.TransformVector(Vector3::UnitZ);
			forward.Normalize();
		}
	};
}
