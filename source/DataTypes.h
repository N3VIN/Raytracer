#pragma once
#include <cassert>

#include "Math.h"
#include "vector"

namespace dae
{
#pragma region GEOMETRY
	struct Sphere
	{
		// Data Types
		Vector3 origin{};
		float radius{};

		unsigned char materialIndex{ 0 };

		// Constructor & Destructor
		Sphere() = default;
		Sphere(const Vector3& _origin, float _radius, unsigned char _materialIndex)
			: origin(_origin)
			, radius(_radius)
			, materialIndex{ _materialIndex } {}
		
		~Sphere() = default;

		Sphere(const Sphere&) = default;
		Sphere(Sphere&&) noexcept = default;
		Sphere& operator=(const Sphere&) = default;
		Sphere& operator=(Sphere&&) noexcept = default;
	};

	struct Plane
	{
		// Data Types
		Vector3 origin{};
		Vector3 normal{};

		unsigned char materialIndex{ 0 };

		// Constructor & Destructor
		Plane() = default;
		Plane(const Vector3& _normal, const Vector3& _origin, unsigned char _materialIndex)
			: origin(_origin)
			, normal(_normal)
			, materialIndex{ _materialIndex } {}

		~Plane() = default;

		Plane(const Plane&) = default;
		Plane(Plane&&) noexcept = default;
		Plane& operator=(const Plane&) = default;
		Plane& operator=(Plane&&) noexcept = default;
	};

	enum class TriangleCullMode
	{
		FrontFaceCulling,
		BackFaceCulling,
		NoCulling
	};

	struct Triangle
	{
		Triangle() = default;
		Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2, const Vector3& _normal):
			v0{_v0}, v1{_v1}, v2{_v2}, normal{_normal.Normalized()}{}

		Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2) :
			v0{ _v0 }, v1{ _v1 }, v2{ _v2 }
		{
			const Vector3 edgeV0V1 = v1 - v0;
			const Vector3 edgeV0V2 = v2 - v0;
			normal = Vector3::Cross(edgeV0V1, edgeV0V2).Normalized();
			center = (v0 + v1 + v2) / 3.f;
		}

		Vector3 v0{};
		Vector3 v1{};
		Vector3 v2{};

		Vector3 normal{};
		Vector3 center{};

		TriangleCullMode cullMode{};
		unsigned char materialIndex{};
	};

	struct TriangleMesh
	{
		TriangleMesh() = default;
		TriangleMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, TriangleCullMode _cullMode):
		positions(_positions), indices(_indices), cullMode(_cullMode)
		{
			/*for (const auto &i : positions)
			{
				center += Vector3{ i };
			}
			center = { center.x / positions.size(), center.y / positions.size(), center.z / positions.size() };*/

			//Calculate Normals
			CalculateNormals();

			//Update Transforms
			UpdateTransforms();

		
		}

		TriangleMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, const std::vector<Vector3>& _normals, TriangleCullMode _cullMode) :
			positions(_positions), indices(_indices), normals(_normals), cullMode(_cullMode)
		{
			/*for (const auto &i : positions)
			{
				center += Vector3{ i };
			}
			center = { center.x / positions.size(), center.y / positions.size(), center.z / positions.size() };*/

			UpdateTransforms();
		}

		std::vector<Vector3> positions{};
		std::vector<Vector3> normals{};
		std::vector<int> indices{};
		unsigned char materialIndex{};
		Vector3 center;


		TriangleCullMode cullMode{TriangleCullMode::BackFaceCulling};

		Matrix rotationTransform{};
		Matrix translationTransform{};
		Matrix scaleTransform{};

		std::vector<Vector3> transformedPositions{};
		std::vector<Vector3> transformedNormals{};

		void Translate(const Vector3& translation)
		{
			translationTransform = Matrix::CreateTranslation(translation);
		}

		void RotateY(float yaw)
		{
			rotationTransform = Matrix::CreateRotationY(yaw);
		}

		void Scale(const Vector3& scale)
		{
			scaleTransform = Matrix::CreateScale(scale);
		}

		void AppendTriangle(const Triangle& triangle, bool ignoreTransformUpdate = false)
		{
			int startIndex = static_cast<int>(positions.size());

			positions.push_back(triangle.v0);
			positions.push_back(triangle.v1);
			positions.push_back(triangle.v2);

			indices.push_back(startIndex);
			indices.push_back(++startIndex);
			indices.push_back(++startIndex);

			normals.push_back(triangle.normal);

			//Not ideal, but making sure all vertices are updated
			if(!ignoreTransformUpdate)
				UpdateTransforms();
		}

		void CalculateNormals()
		{
			normals.clear();
			Vector3 normal{};
			for (size_t i = 0; i < indices.size(); i += 3)
			{
				const Vector3 v0 = positions[indices[i]];
				const Vector3 v1 = positions[indices[i + 1]];
				const Vector3 v2 = positions[indices[i + 2]];

				Vector3 a{ v1 - v0 };
				Vector3 b{ v2 - v0 };
				normal = Vector3::Cross(a, b).Normalized();
				normals.push_back(normal);
			}
		}

		void UpdateTransforms()
		{
			transformedPositions.clear();
			transformedNormals.clear();

			const auto finalTransform{ translationTransform * rotationTransform * scaleTransform };
			//const auto finalTransform{ translationTransform * rotationTransform };

			transformedPositions.reserve(positions.size());
			for (size_t i = 0; i < positions.size(); i++)
			{
				//positions[i] = positions[i] - center;
				//transformedPositions.emplace_back(finalTransform.TransformPoint(positions[i]) + center);

				transformedPositions.emplace_back(finalTransform.TransformPoint(positions[i]));

				//transformedPositions.emplace_back() = positions[i];
			}

			transformedNormals.reserve(normals.size());
			for (size_t i = 0; i < normals.size(); i++)
			{
				transformedNormals.emplace_back( finalTransform.TransformVector(normals[i]));

				//transformedNormals.emplace_back() = normals[i];
			}
		}
	};
#pragma endregion
#pragma region LIGHT
	enum class LightType
	{
		Point,
		Directional
	};

	struct Light
	{
		Vector3 origin{};
		Vector3 direction{};
		ColorRGB color{};
		float intensity{};

		LightType type{};
	};
#pragma endregion
#pragma region MISC
	struct Ray
	{
		// Data Types
		Vector3 origin{};
		Vector3 direction{};

		float min{ 0.0001f };
		float max{ FLT_MAX };

		// Constructor & Destructor
		Ray(const Vector3& _origin, const Vector3& _direction)
			: origin(_origin), direction(_direction) {}
		~Ray() = default;

		Ray(const Ray&) = default;
		Ray(Ray&&) noexcept = default;
		Ray& operator=(const Ray&) = default;
		Ray& operator=(Ray&&) noexcept = default;

	};

	struct HitRecord
	{
		Vector3 origin{};
		Vector3 normal{};
		float t = FLT_MAX;

		bool didHit{ false };
		unsigned char materialIndex{ 0 };
	};
#pragma endregion
}