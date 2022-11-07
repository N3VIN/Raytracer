//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
	m_AspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);

}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	float fov = camera.fovAngle;

	Vector3 right{Vector3::UnitX}, up{Vector3::UnitY}, look{Vector3::UnitZ};

	//Look at matrix.
	const Matrix cameraToWorld{ camera.CalculateCameraToWorld() };

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			//Raster Space.
			float x{ float((2 * ((px + 0.5f) / m_Width) - 1)) * (fov * m_AspectRatio) };
			float y{ float(1 - 2 * ((py + 0.5f) / m_Height)) * fov };

			Vector3 forwardVec{ x, y, 1 };

			//RayDirection calculations.
			//Vector3 rayDirection{ (x * right) + (y * up) + look};
			//Vector3 rayDirection{ transformedVector.GetAxisX().x, transformedVector.GetAxisY().y, transformedVector.GetAxisZ().z};
			Vector3 rayDirection{ cameraToWorld.TransformVector(forwardVec.Normalized())};
			rayDirection.Normalized();

			Ray viewRay{ camera.origin, rayDirection };
			ColorRGB finalColor{};

			HitRecord closestHit{};


			pScene->GetClosestHit(viewRay, closestHit);

			if (closestHit.didHit)
			{
				for (const auto& i : lights)
				{
					//finalColor = materials[closestHit.materialIndex]->Shade();


					//bool isPointVisible{ true };
					//Vector3 startPoint{ closestHit.origin + closestHit.normal * 0.01f };
					//Vector3 direction{ LightUtils::GetDirectionToLight(i, startPoint) };
					//Ray lightRay{ startPoint, direction };
					////Ray lightRay{ closestHit.origin, LightUtils::GetDirectionToLight(i, closestHit.origin) };
					//lightRay.max = LightUtils::GetDirectionToLight(i, closestHit.origin).Normalize();
					//isPointVisible = pScene->DoesHit(lightRay);

					//if (isPointVisible)
					//{
						//finalColor = materials[closestHit.materialIndex]->Shade() * 0.5f;

					//}
					//else
					//{
					switch (m_CurrentLightingMode)
					{
					case dae::Renderer::LightingMode::ObservedArea:
						finalColor += ColorRGB({1.f, 1.f, 1.f}) * GetLambertCosine(closestHit.normal, LightUtils::GetDirectionToLight(i, closestHit.origin));
						break;
					case dae::Renderer::LightingMode::Radiance:
						finalColor += LightUtils::GetRadiance(i, closestHit.origin);
						break;
					case dae::Renderer::LightingMode::BRDF:
						finalColor += materials[closestHit.materialIndex]->Shade(closestHit, LightUtils::GetDirectionToLight(i, closestHit.origin).Normalized(), rayDirection);
						break;
					case dae::Renderer::LightingMode::Combined:
						finalColor += LightUtils::GetRadiance(i, closestHit.origin)
							* materials[closestHit.materialIndex]->Shade(closestHit, LightUtils::GetDirectionToLight(i, closestHit.origin).Normalized(), rayDirection)
							* GetLambertCosine(closestHit.normal, LightUtils::GetDirectionToLight(i, closestHit.origin));
						break;				
					}
	
				}
			}
			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));

			//finalColor = colors::Black;
		}
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void dae::Renderer::CycleLightingMode()
{
	int count{ static_cast<int>(m_CurrentLightingMode) };
	count++;
	if (count > 3)
	{
		count = 0;
	}
	LightingMode castEnum = static_cast<LightingMode>(count);
	m_CurrentLightingMode = castEnum;
}



float Renderer::GetLambertCosine(const dae::Vector3& normal, const dae::Vector3& lightDirection) const
{
	float lambertCosine{};
	lambertCosine = Vector3::Dot(normal, lightDirection.Normalized());
	if (lambertCosine < 0)
	{
		return 0.0f;
		//return lambertCosine;
	}
	else
	{
		return lambertCosine;
	}


}