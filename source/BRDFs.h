#pragma once
#include <cassert>
#include "Math.h"

namespace dae
{
	namespace BRDF
	{
		/**
		 * \param kd Diffuse Reflection Coefficient
		 * \param cd Diffuse Color
		 * \return Lambert Diffuse Color
		 */
		static ColorRGB Lambert(float kd, const ColorRGB& cd)
		{
			ColorRGB lambert{ (cd * kd) / float(PI) };
			return lambert;
		}

		static ColorRGB Lambert(const ColorRGB& kd, const ColorRGB& cd)
		{
			// not sure.
			ColorRGB lambert{ (cd * kd) / float(PI) };
			return lambert;
		}

		/**
		 * \brief todo
		 * \param ks Specular Reflection Coefficient
		 * \param exp Phong Exponent
		 * \param l Incoming (incident) Light Direction
		 * \param v View Direction
		 * \param n Normal of the Surface
		 * \return Phong Specular Color
		 */
		static ColorRGB Phong(float ks, float exp, const Vector3& l, const Vector3& v, const Vector3& n)
		{
			Vector3 reflect{ Vector3::Reflect(l, n) };
			float cosAlpha{ Vector3::Dot(reflect, v) };
			float phong{ ks * (pow(cosAlpha, exp)) };
			return ColorRGB(phong, phong, phong);
		}

		/**
		 * \brief BRDF Fresnel Function >> Schlick
		 * \param h Normalized Halfvector between View and Light directions
		 * \param v Normalized View direction
		 * \param f0 Base reflectivity of a surface based on IOR (Indices Of Refrection), this is different for Dielectrics (Non-Metal) and Conductors (Metal)
		 * \return
		 */
		static ColorRGB FresnelFunction_Schlick(const Vector3& h, const Vector3& v, const ColorRGB& f0)
		{
			//Vector3 halfVector{ (v + h) / (v + h).Magnitude() };
			ColorRGB fresnel{};
			float hDotV{ std::max(Vector3::Dot(h, v) , 0.f) };
			fresnel = f0 + (ColorRGB{ 1, 1, 1 } - f0) * (powf(1 - hDotV, 5));

			return fresnel;
		}

		/**
		 * \brief BRDF NormalDistribution >> Trowbridge-Reitz GGX (UE4 implemetation - squared(roughness))
		 * \param n Surface normal
		 * \param h Normalized half vector
		 * \param roughness Roughness of the material
		 * \return BRDF Normal Distribution Term using Trowbridge-Reitz GGX
		 */
		static float NormalDistribution_GGX(const Vector3& n, const Vector3& h, float roughness)
		{
			float normalDistribution{};
			float roughnessSqr{ powf(roughness, 2) };
			float numerator{ powf(roughnessSqr, 2) };
			float nDotH{ std::max(Vector3::Dot(n, h), 0.f) };
			float denominator{ static_cast<float>(PI) * powf(powf(nDotH, 2) * (numerator - 1) + 1, 2) };
			normalDistribution = numerator / denominator;
			return normalDistribution;
		}


		/**
		 * \brief BRDF Geometry Function >> Schlick GGX (Direct Lighting + UE4 implementation - squared(roughness))
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using SchlickGGX
		 */
		static float GeometryFunction_SchlickGGX(const Vector3& n, const Vector3& v, float roughness)
		{
			float geometryFunction{};
			float roughnessSqr{ powf(roughness, 2) };
			float directRoughness{ (powf((roughnessSqr + 1), 2)) / 8.f };
			float dotNV{ std::max(Vector3::Dot(n, v), 0.f) };
			geometryFunction = dotNV / (dotNV * (1 - directRoughness) + directRoughness);
			return geometryFunction;
		}

		/**
		 * \brief BRDF Geometry Function >> Smith (Direct Lighting)
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param l Normalized light direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using Smith (> SchlickGGX(n,v,roughness) * SchlickGGX(n,l,roughness))
		 */
		static float GeometryFunction_Smith(const Vector3& n, const Vector3& v, const Vector3& l, float roughness)
		{
			return GeometryFunction_SchlickGGX(n, v, roughness) * GeometryFunction_SchlickGGX(n, l, roughness);

		}

	}
}