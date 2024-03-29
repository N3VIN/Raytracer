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

Renderer::Renderer(SDL_Window* pWindow) :
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

	camera.CalculateCameraToWorld();

	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	const float fov = camera.fovAngle;

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
}

void dae::Renderer::RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Camera& camera, const std::vector<Light>& lights, const std::vector<Material*>& materials) const
{
	const int px = pixelIndex % m_Width;
	const int py = pixelIndex / m_Width;

	const float rx = px + 0.5f;
	const float ry = py + 0.5f;

	const float cx = (2 * (rx / float(m_Width)) - 1) * aspectRatio * fov;
	const float cy = (1 - (2 * (ry / float(m_Height)))) * fov;

	const Vector3 forwardVec{ cx, cy, 1 };

	const Vector3 rayDirection{ camera.cameraToWorld.TransformVector(forwardVec.Normalized())};
	rayDirection.Normalized();

	Ray viewRay{ camera.origin, rayDirection };
	ColorRGB finalColor{};
	float lambda = 1.0f;
	float reflectivity{};

	for (int bounce = 0; bounce <= m_NumBounces; bounce++)
	{
		HitRecord closestHit{};
		pScene->GetClosestHit(viewRay, closestHit);

		if (closestHit.didHit)
		{
			for (const auto& i : lights)
			{
				const Vector3 startPoint{ closestHit.origin + closestHit.normal * 0.01f };
				const Vector3 direction{ LightUtils::GetDirectionToLight(i, startPoint) };
				Ray lightRay{ startPoint, direction.Normalized() };
				lightRay.min = 0.0001f;
				lightRay.max = direction.Magnitude();
				bool occluderHit{ false };

				const auto lambertCosine{ GetLambertCosine(closestHit.normal, LightUtils::GetDirectionToLight(i, closestHit.origin)) };
				const auto radiance{ LightUtils::GetRadiance(i, closestHit.origin) };
				const auto brdf{ materials[closestHit.materialIndex]->Shade(closestHit, LightUtils::GetDirectionToLight(i, closestHit.origin).Normalized(), rayDirection) };

				if (m_ShadowsEnabled)
				{
					occluderHit = pScene->DoesHit(lightRay);
				}
				if (!occluderHit)
				{
					switch (m_CurrentLightingMode)
					{
					case dae::Renderer::LightingMode::ObservedArea:
						finalColor += ColorRGB({ 1.f, 1.f, 1.f }) * lambertCosine;
						break;
					case dae::Renderer::LightingMode::Radiance:
						finalColor += radiance;
						break;
					case dae::Renderer::LightingMode::BRDF:
						finalColor += brdf;
						break;
					case dae::Renderer::LightingMode::Combined:
						if(bounce > 0)
						{
							finalColor += radiance
								* brdf
								* lambertCosine
								* reflectivity
								* lambda;
						}
						else
						{
							finalColor += radiance
								* brdf
								* lambertCosine;
						}
						break;
					}
				}
			}

			if (m_ReflectionsEnabled)
			{
				reflectivity = materials[closestHit.materialIndex]->GetReflectivity();
				lambda *= 0.7f;
				viewRay.origin = closestHit.origin + closestHit.normal * 0.01f;
				viewRay.direction = Vector3::Reflect(viewRay.direction, closestHit.normal);
			}
			if (!m_ReflectionsEnabled || reflectivity < FLT_EPSILON)
			{
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
	lambertCosine = std::max( Vector3::Dot(normal, lightDirection.Normalized()), 0.0f);
	return lambertCosine;
}