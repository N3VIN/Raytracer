#include "Scene.h"
#include "Utils.h"
#include "Material.h"

namespace dae {

#pragma region Base Scene
	//Initialize Scene with Default Solid Color Material (RED)
	Scene::Scene():
		m_Materials({ new Material_SolidColor({1,0,0})})
	{
		m_SphereGeometries.reserve(32);
		m_PlaneGeometries.reserve(32);
		m_TriangleMeshGeometries.reserve(32);
		m_Lights.reserve(32);
	}

	Scene::~Scene()
	{
		for(auto& pMaterial : m_Materials)
		{
			delete pMaterial;
			pMaterial = nullptr;
		}

		m_Materials.clear();
	}

	void dae::Scene::GetClosestHit(const Ray& ray, HitRecord& closestHit) const
	{
		HitRecord hit{};

		// doesnt work for multiple plane geometries. Probabaly because t value issue?

		for (const auto& i : m_PlaneGeometries)
		{
			if (GeometryUtils::HitTest_Plane(i, ray, closestHit))
			{
				if (closestHit.t < hit.t)
				{
					hit = closestHit;
				}
				else
				{
					closestHit = hit;
				}
			}
		}

		//..

		for (const auto &i : m_SphereGeometries)
		{
			// use the new hit for the intersection.
			if (GeometryUtils::HitTest_Sphere(i, ray, closestHit))
			{
				if (closestHit.t < hit.t)
				{
					hit = closestHit;
				}
				else
				{
					closestHit = hit;
				}
			}	
		}
		
	}

	bool Scene::DoesHit(const Ray& ray) const
	{
		HitRecord tempHitRecord{};

		for (const auto& i : m_PlaneGeometries)
		{
			if (GeometryUtils::HitTest_Plane(i, ray, tempHitRecord, true))
			{
				return true;
			}

		}

		//..

		for (const auto& i : m_SphereGeometries)
		{
			if (GeometryUtils::HitTest_Sphere(i, ray, tempHitRecord, true))
			{
				return true;
			}
		}

		return false;

	}

#pragma region Scene Helpers
	Sphere* Scene::AddSphere(const Vector3& origin, float radius, unsigned char materialIndex)
	{
		Sphere s;
		s.origin = origin;
		s.radius = radius;
		s.materialIndex = materialIndex;

		m_SphereGeometries.emplace_back(s);
		return &m_SphereGeometries.back();
	}

	Plane* Scene::AddPlane(const Vector3& origin, const Vector3& normal, unsigned char materialIndex)
	{
		Plane p;
		p.origin = origin;
		p.normal = normal;
		p.materialIndex = materialIndex;

		m_PlaneGeometries.emplace_back(p);
		return &m_PlaneGeometries.back();
	}

	TriangleMesh* Scene::AddTriangleMesh(TriangleCullMode cullMode, unsigned char materialIndex)
	{
		TriangleMesh m{};
		m.cullMode = cullMode;
		m.materialIndex = materialIndex;

		m_TriangleMeshGeometries.emplace_back(m);
		return &m_TriangleMeshGeometries.back();
	}

	Light* Scene::AddPointLight(const Vector3& origin, float intensity, const ColorRGB& color)
	{
		Light l;
		l.origin = origin;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Point;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	Light* Scene::AddDirectionalLight(const Vector3& direction, float intensity, const ColorRGB& color)
	{
		Light l;
		l.direction = direction;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Directional;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	unsigned char Scene::AddMaterial(Material* pMaterial)
	{
		m_Materials.push_back(pMaterial);
		return static_cast<unsigned char>(m_Materials.size() - 1);
	}
#pragma endregion
#pragma endregion

#pragma region SCENE W1
	void Scene_W1::Initialize()
	{
				//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//Spheres
		AddSphere({ -25.f, 0.f, 100.f }, 50.f, matId_Solid_Red);
		AddSphere({ 25.f, 0.f, 100.f }, 50.f, matId_Solid_Blue);

		//Plane
		AddPlane({ -75.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 75.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.f, -75.f, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 75.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 125.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);
	}
#pragma endregion

#pragma region SCENE W2
	void Scene_W2::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.updateFovAngle(45.f);

		//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//Spheres
		AddSphere({ -1.75f, 1.f, 0.f }, 0.75f, matId_Solid_Red);
		AddSphere({ 0.f, 1.f, 0.f }, 0.75f, matId_Solid_Blue);
		AddSphere({ 1.75f, 1.f, 0.f }, 0.75f, matId_Solid_Red);
		AddSphere({ -1.75f, 3.f, 0.f }, 0.75f, matId_Solid_Blue);
		AddSphere({ 0.f, 3.f, 0.f }, 0.75f, matId_Solid_Red);
		AddSphere({ 1.75f, 3.f, 0.f }, 0.75f, matId_Solid_Blue);
		

		//Plane
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);

		//Light
		AddPointLight({ 0.f, 5.f, -5.f }, 70.f, colors::White);
	}
#pragma endregion

#pragma region SCENE W3
	void Scene_W3::Initialize()
	{
		//m_Camera.origin = { 0.f, 3.f, -9.f };
		//m_Camera.updateFovAngle(45.f);

		//////default: Material id0 >> SolidColor Material (RED)
		////constexpr unsigned char matId_Solid_Red = 0;
		////const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		////const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		////const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		////const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//const auto matCT_GreyRoughMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.f, 1.f));
		//const auto matCT_GreyMediumMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.f, .6f));
		//const auto matCT_GreySmoothMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.f, .1f));
		//const auto matCT_GreyRoughPlastic = AddMaterial(new Material_CookTorrence({ 0.75f, 0.75f, 0.75f}, 0.0f, 1.f));
		//const auto matCT_GreyMediumPlastic = AddMaterial(new Material_CookTorrence({0.75f, 0.75f, 0.75f },0.0f, .6f));
		//const auto matCT_GreySmoothPlastic = AddMaterial(new Material_CookTorrence({0.75f, 0.75f, 0.75f },0.0f, .1f));

		//const auto matLambert_GreyBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.57f }, 1.f));

		////Spheres
		//AddSphere({ -1.75f, 1.f, 0.f }, 0.75f, matCT_GreyRoughMetal);
		//AddSphere({ 0.f, 1.f, 0.f }, 0.75f, matCT_GreyRoughMetal);
		//AddSphere({ 1.75f, 1.f, 0.f }, 0.75f, matCT_GreySmoothMetal);
		//AddSphere({ -1.75f, 3.f, 0.f }, 0.75f, matCT_GreyRoughPlastic);
		//AddSphere({ 0.f, 3.f, 0.f }, 0.75f, matCT_GreyMediumPlastic);
		//AddSphere({ 1.75f, 3.f, 0.f }, 0.75f, matCT_GreySmoothPlastic);


		////Plane
		//AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GreyBlue);
		//AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GreyBlue);
		//AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GreyBlue);
		//AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GreyBlue);
		//AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GreyBlue);

		////Light
		//AddPointLight({ 0.f, 5.f, 5.f }, 50.f, ColorRGB{1.f, 0.61f, 0.45f});
		//AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{1.f, 0.8f, 0.45f});
		//AddPointLight({ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{0.34f, 0.47f, 0.68f});

		//..

		m_Camera.origin = { 0.f, 1.f, -5.f };
		m_Camera.updateFovAngle(45.f);

		const unsigned char matLambert_red = AddMaterial(new Material_Lambert{ colors::Red, 1.f });
		const unsigned char matLambertPhong_Blue = AddMaterial(new Material_LambertPhong{ colors::Blue, 1.f, 1.f, 60.f });
		const unsigned char matLambert_Yellow = AddMaterial(new Material_Lambert{ colors::Yellow, 1.f });

		AddSphere({ -.75f, 1.f, 0.f }, 1.f, matLambert_red);
		AddSphere({ .75f, 1.f, 0.f }, 1.f, matLambertPhong_Blue);

		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 1.f, 0.f }, matLambert_Yellow);

		AddPointLight({ 0.f, 5.f, 5.f }, 25.f, colors::White);
		AddPointLight({ 0.f, 2.5f, -5.f }, 25.f, colors::White);

	}
#pragma endregion
}
