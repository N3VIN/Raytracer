//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
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

	// Testing the dot and cross product.
	//float dotResult{};
	//dotResult = Vector3::Dot(Vector3::UnitX, Vector3::UnitX);
	//dotResult = Vector3::Dot(Vector3::UnitX, -Vector3::UnitX);
	//dotResult = Vector3::Dot(Vector3::UnitX, Vector3::UnitY);

	//Vector3 crossResult{};
	//crossResult = Vector3::Cross(Vector3::UnitZ, Vector3::UnitX);
	//crossResult = Vector3::Cross(Vector3::UnitX, Vector3::UnitZ);

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

			Matrix defaultForwardVector{ 
				Vector4{x, 0, 0, 0}, 
				Vector4{0, y, 0, 0}, 
				Vector4{0, 0, 1, 0}, 
				Vector4{0, 0, 0, 0} };
			Matrix transformedVector{ cameraToWorld * defaultForwardVector };

			//RayDirection calculations.
			//Vector3 rayDirection{ (x * right) + (y * up) + look};
			Vector3 rayDirection{ transformedVector.GetAxisX().x, transformedVector.GetAxisY().y, transformedVector.GetAxisZ().z};
			rayDirection.Normalize();

			Ray viewRay{ camera.origin, rayDirection };
			ColorRGB finalColor{};

			HitRecord closestHit{};

			/*Sphere testSphere{ {0.0f, 0.0f, 100.0f}, 50.0f, 0 };
			GeometryUtils::HitTest_Sphere(testSphere, viewRay, closestHit);*/

			/*Plane testPlane{ {0.0f, -50.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 0 };
			GeometryUtils::HitTest_Plane(testPlane, viewRay, closestHit);*/

			pScene->GetClosestHit(viewRay, closestHit);

			if (closestHit.didHit)
			{
				finalColor = materials[closestHit.materialIndex]->Shade();
				/*const float scaled_t = closestHit.t / 500.f;
				finalColor = { scaled_t, scaled_t, scaled_t };*/
			}

			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
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
