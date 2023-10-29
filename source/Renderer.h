#pragma once

#include <cstdint>
#include "Math.h"
#include <vector>


struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	struct Camera;
	struct Light;
	class Material;
	class Scene;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(Scene* pScene) const;
		void RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Camera& camera, const std::vector<Light>& lights, const std::vector<Material*>& materials) const;
		bool SaveBufferToImage() const;

		void CycleLightingMode();
		void ToggleShadows()
		{
			m_ShadowsEnabled = !m_ShadowsEnabled;
		}
		void ToggleReflections()
		{
			m_ReflectionsEnabled = !m_ReflectionsEnabled;
		}

	private:
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};

		int m_Width{};
		int m_Height{};
		int m_NumBounces{10};
		float m_AspectRatio{};

		enum class LightingMode
		{
			ObservedArea, // Lambert Cosine Law
			Radiance, // Incident Radiance
			BRDF, // Scattering of the Light
			Combined // ObservedArea * Radiance * BRDF
		};

		LightingMode m_CurrentLightingMode{ LightingMode::Combined };
		bool m_ShadowsEnabled{ true };
		bool m_ReflectionsEnabled{ true };

		//

		float GetLambertCosine(const dae::Vector3& normal, const dae::Vector3& lightDirection) const;


	};
}
