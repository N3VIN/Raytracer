#pragma once

#include <cstdint>
#include "Math.h"



struct SDL_Window;
struct SDL_Surface;

namespace dae
{
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
		bool SaveBufferToImage() const;

		void CycleLightingMode();
		void ToggleShadows()
		{
			m_ShadowsEnabled = !m_ShadowsEnabled;
		}

	private:
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};

		int m_Width{};
		int m_Height{};
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

		//

		float GetLambertCosine(const dae::Vector3& normal, const dae::Vector3& lightDirection) const;


	};
}
