#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"


namespace dae
{
	//using namespace Vector3;

	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			float a{ Vector3::Dot(ray.direction, ray.direction) };
			float b{ Vector3::Dot((ray.direction * 2), (ray.origin - sphere.origin)) };
			//float b{ Dot((ray.origin - m_Point), ray.direction * 2.0f) };
			float c{ (Vector3::Dot((ray.origin - sphere.origin), (ray.origin - sphere.origin))) - (sphere.radius * sphere.radius) };

			float discriminant{ (b * b) - (4 * (a * c)) };

			float t{};


			if (discriminant > 0)
			{


				t = ((-b) + sqrt(discriminant)) / (2 * a);

				if (t <= ray.min)
				{
					t = ((-b) - sqrt(discriminant)) / (2 * a);

				}

				t = ((-b) - sqrt(discriminant)) / (2 * a);

				if (t >= ray.min && t <= ray.max)
				{
					if (ignoreHitRecord)
					{
						return true;
					}

					hitRecord.materialIndex = sphere.materialIndex;
					hitRecord.origin = ray.origin + (t * ray.direction);
					hitRecord.didHit = true;
					hitRecord.t = t;
					hitRecord.normal = (hitRecord.origin - sphere.origin).Normalized();
					return true;
				}
				else
				{
					hitRecord.didHit = false;
					return false;
				}

			}
			else
			{
				return false;
			}
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			float t{ Vector3::Dot((plane.origin - ray.origin), plane.normal) / Vector3::Dot(ray.direction, plane.normal) };

			if (t >= ray.min && t <= ray.max)
			{
				if (ignoreHitRecord)
				{
					return true;
				}

				hitRecord.materialIndex = plane.materialIndex;
				hitRecord.origin = ray.origin + (t * ray.direction);
				hitRecord.didHit = true;
				hitRecord.t = t;
				hitRecord.normal = plane.normal;
				return true;
				
			}
			else
			{
				// Dont know why it works the other way around.
				hitRecord.didHit = false;
				return false;
			}
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			assert(false && "No Implemented Yet!");
			return false;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			assert(false && "No Implemented Yet!");
			return false;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			return {light.origin - origin};
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			return  light.color * (light.intensity / Vector3::Dot(light.origin - target, light.origin - target));

		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof()) 
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}