//External includes
#include "SDL.h"
#include "SDL_surface.h"
#include <future> //async
#include <ppl.h> // parallel_for

//Project includes
#include "Renderer.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

//#define ASYNC
#define PARALLEL_FOR

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
	//const Matrix cameraToWorld{ camera.CalculateCameraToWorld() };

	camera.CalculateCameraToWorld();

	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	float fov = camera.fovAngle;

	const uint32_t numPixels = m_Width * m_Height;

#if defined(ASYNC)
	// Async logic

	const uint32_t numCores = std::thread::hardware_concurrency();
	std::vector<std::future<void>> async_futures{};
	const uint32_t numPixelsPerTask = numPixels / numCores;
	uint32_t numUnassignedPixels = numPixels % numCores;
	uint32_t currPixelIndex = 0;

	for (uint32_t coreId = 0; coreId < numCores; ++coreId)
	{
		uint32_t taskSize = numPixelsPerTask;
		if (numUnassignedPixels > 0)
		{
			++taskSize;
			--numUnassignedPixels;
		}

		async_futures.push_back(std::async(std::launch::async, [=, this]
			{
				//Render all the pixels for this task.
				const uint32_t pixelIndexEnd = currPixelIndex + taskSize;
				for (uint32_t pixelIndex = currPixelIndex; pixelIndex < pixelIndexEnd; ++pixelIndex)
				{
					RenderPixel(pScene, pixelIndex, fov, m_AspectRatio, camera, lights, materials);
				}
			}));

		currPixelIndex += taskSize;
	}

	//Wait for async completion of all tasks.
	for (const std::future<void>& f : async_futures)
	{
		f.wait();
	}

#elif defined(PARALLEL_FOR)
	// Parallel-For Logic

	concurrency::parallel_for(0u, numPixels, [=, this](int i)
		{
			RenderPixel(pScene, i, fov, m_AspectRatio, camera, lights, materials);
		});

#else
	// Synchronous Logic (no threading)

	for (uint32_t i = 0; i < numPixels; ++i)
	{
		RenderPixel(pScene, i, fov, m_AspectRatio, camera, lights, materials);
	}
#endif

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);

	//.....

	//Vector3 right{Vector3::UnitX}, up{Vector3::UnitY}, look{Vector3::UnitZ};

	//Look at matrix.
	//const Matrix cameraToWorld{ camera.CalculateCameraToWorld() };

	//for (int px{}; px < m_Width; ++px)
	//{
	//	for (int py{}; py < m_Height; ++py)
	//	{
	//		//Raster Space.
	//		float x{ float((2 * ((px + 0.5f) / m_Width) - 1)) * (fov * m_AspectRatio) };
	//		float y{ float(1 - 2 * ((py + 0.5f) / m_Height)) * fov };

	//		Vector3 forwardVec{ x, y, 1 };

	//		//RayDirection calculations.
	//		//Vector3 rayDirection{ (x * right) + (y * up) + look};
	//		//Vector3 rayDirection{ transformedVector.GetAxisX().x, transformedVector.GetAxisY().y, transformedVector.GetAxisZ().z};
	//		Vector3 rayDirection{ cameraToWorld.TransformVector(forwardVec.Normalized())};
	//		rayDirection.Normalized();

	//		Ray viewRay{ camera.origin, rayDirection };
	//		ColorRGB finalColor{};

	//		HitRecord closestHit{};


	//		pScene->GetClosestHit(viewRay, closestHit);

	//		if (closestHit.didHit)
	//		{
	//			for (const auto& i : lights)
	//			{
	//				bool occluderHit{ false };
	//				if (m_ShadowsEnabled)
	//				{
	//					Vector3 startPoint{ closestHit.origin + closestHit.normal * 0.01f };
	//					Vector3 direction{ LightUtils::GetDirectionToLight(i, startPoint) };
	//					Ray lightRay{ startPoint, direction.Normalized() };
	//					lightRay.min = 0.0001f;
	//					lightRay.max = direction.Magnitude();
	//					occluderHit = pScene->DoesHit(lightRay);
	//				}
	//				if (!occluderHit)
	//				{
	//					switch (m_CurrentLightingMode)
	//					{
	//					case dae::Renderer::LightingMode::ObservedArea:
	//						finalColor += ColorRGB({ 1.f, 1.f, 1.f }) * GetLambertCosine(closestHit.normal, LightUtils::GetDirectionToLight(i, closestHit.origin));
	//						break;
	//					case dae::Renderer::LightingMode::Radiance:
	//						finalColor += LightUtils::GetRadiance(i, closestHit.origin);
	//						break;
	//					case dae::Renderer::LightingMode::BRDF:
	//						finalColor += materials[closestHit.materialIndex]->Shade(closestHit, LightUtils::GetDirectionToLight(i, closestHit.origin).Normalized(), rayDirection);
	//						break;
	//					case dae::Renderer::LightingMode::Combined:
	//						finalColor += LightUtils::GetRadiance(i, closestHit.origin)
	//							* materials[closestHit.materialIndex]->Shade(closestHit, LightUtils::GetDirectionToLight(i, closestHit.origin).Normalized(), rayDirection)
	//							* GetLambertCosine(closestHit.normal, LightUtils::GetDirectionToLight(i, closestHit.origin));
	//						break;
	//					}
	//				}
	//			}
	//		}
	//		//Update Color in Buffer
	//		finalColor.MaxToOne();

	//		m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
	//			static_cast<uint8_t>(finalColor.r * 255),
	//			static_cast<uint8_t>(finalColor.g * 255),
	//			static_cast<uint8_t>(finalColor.b * 255));

	//	}
	//}

	//@END
	//Update SDL Surface
	//SDL_UpdateWindowSurface(m_pWindow);
}

void dae::Renderer::RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Camera& camera, const std::vector<Light>& lights, const std::vector<Material*>& materials) const
{
	const int px = pixelIndex % m_Width;
	const int py = pixelIndex / m_Width;

	float rx = px + 0.5f;
	float ry = py + 0.5f;

	float cx = (2 * (rx / float(m_Width)) - 1) * aspectRatio * fov;
	float cy = (1 - (2 * (ry / float(m_Height)))) * fov;

	Vector3 forwardVec{ cx, cy, 1 };

	//RayDirection calculations.
	//Vector3 rayDirection{ (x * right) + (y * up) + look};
	//Vector3 rayDirection{ transformedVector.GetAxisX().x, transformedVector.GetAxisY().y, transformedVector.GetAxisZ().z};
	
	//Vector3 rayDirection{ cameraToWorld.TransformVector(forwardVec.Normalized()) };
	Vector3 rayDirection{ camera.cameraToWorld.TransformVector(forwardVec.Normalized())};
	rayDirection.Normalized();

	Ray viewRay{ camera.origin, rayDirection };
	ColorRGB finalColor{};

	HitRecord closestHit{};


	pScene->GetClosestHit(viewRay, closestHit);

	if (closestHit.didHit)
	{
		for (const auto& i : lights)
		{
			bool occluderHit{ false };
			if (m_ShadowsEnabled)
			{
				Vector3 startPoint{ closestHit.origin + closestHit.normal * 0.01f };
				Vector3 direction{ LightUtils::GetDirectionToLight(i, startPoint) };
				Ray lightRay{ startPoint, direction.Normalized() };
				lightRay.min = 0.0001f;
				lightRay.max = direction.Magnitude();
				occluderHit = pScene->DoesHit(lightRay);
			}
			if (!occluderHit)
			{
				switch (m_CurrentLightingMode)
				{
				case dae::Renderer::LightingMode::ObservedArea:
					finalColor += ColorRGB({ 1.f, 1.f, 1.f }) * GetLambertCosine(closestHit.normal, LightUtils::GetDirectionToLight(i, closestHit.origin));
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
	}
	//Update Color in Buffer
	finalColor.MaxToOne();

	m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
		static_cast<uint8_t>(finalColor.r * 255),
		static_cast<uint8_t>(finalColor.g * 255),
		static_cast<uint8_t>(finalColor.b * 255));

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