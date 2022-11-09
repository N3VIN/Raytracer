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
		for (const auto& i : m_PlaneGeometries)
		{
			if (GeometryUtils::HitTest_Plane(i, ray, hit))
			{
				if (hit.t < closestHit.t)
				{
					closestHit = hit;
				}
			}
		}

		//..

		for (const auto &i : m_SphereGeometries)
		{
			// use the new hit for the intersection.
			if (GeometryUtils::HitTest_Sphere(i, ray, hit))
			{
				if (hit.t < closestHit.t)
				{
					closestHit = hit;
				}
			}	
		}

		//..

		//}

		for (const auto& i : m_TriangleMeshGeometries)
		{
			// use the new hit for the intersection.
			if (GeometryUtils::HitTest_TriangleMesh(i, ray, hit))
			{
				if (hit.t < closestHit.t)
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

		//..

		for (const auto& i : m_TriangleMeshGeometries)
		{
			if (GeometryUtils::HitTest_TriangleMesh(i, ray, tempHitRecord, true))
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
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.updateFovAngle(45.f);

		const auto matCT_GreyRoughMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.f, 1.f));
		const auto matCT_GreyMediumMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.f, .6f));
		const auto matCT_GreySmoothMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.f, .1f));
		const auto matCT_GreyRoughPlastic = AddMaterial(new Material_CookTorrence({ 0.75f, 0.75f, 0.75f}, 0.0f, 1.f));
		const auto matCT_GreyMediumPlastic = AddMaterial(new Material_CookTorrence({0.75f, 0.75f, 0.75f },0.0f, .6f));
		const auto matCT_GreySmoothPlastic = AddMaterial(new Material_CookTorrence({0.75f, 0.75f, 0.75f },0.0f, .1f));

		const auto matLambert_GreyBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.57f }, 1.f));

		/*const auto matLambertPhong1 = AddMaterial(new Material_LambertPhong{ colors::Blue, 0.5f, 0.5f, 3.f });
		const auto matLambertPhong2 = AddMaterial(new Material_LambertPhong{ colors::Blue, 0.5f, 0.5f, 15.f });
		const auto matLambertPhong3 = AddMaterial(new Material_LambertPhong{ colors::Blue, 0.5f, 0.5f, 50.f });*/

		/*AddSphere({ -1.75f, 1.f, 0.f }, 0.75f, matLambertPhong1);
		AddSphere({ 0.f, 1.f, 0.f }, 0.75f, matLambertPhong2);
		AddSphere({ 1.75f, 1.f, 0.f }, 0.75f, matLambertPhong3);*/

		//Spheres
		AddSphere({ -1.75f, 1.f, 0.f }, 0.75f, matCT_GreyRoughMetal);
		AddSphere({ 0.f, 1.f, 0.f }, 0.75f, matCT_GreyMediumMetal);
		AddSphere({ 1.75f, 1.f, 0.f }, 0.75f, matCT_GreySmoothMetal);
		AddSphere({ -1.75f, 3.f, 0.f }, 0.75f, matCT_GreyRoughPlastic);
		AddSphere({ 0.f, 3.f, 0.f }, 0.75f, matCT_GreyMediumPlastic);
		AddSphere({ 1.75f, 3.f, 0.f }, 0.75f, matCT_GreySmoothPlastic);


		//Plane
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GreyBlue);
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GreyBlue);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GreyBlue);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GreyBlue);
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GreyBlue);

		//Light
		AddPointLight({ 0.f, 5.f, 5.f }, 50.f, ColorRGB{1.f, 0.61f, 0.45f});
		AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{1.f, 0.8f, 0.45f});
		AddPointLight({ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{0.34f, 0.47f, 0.68f});

	}
#pragma endregion

#pragma region SCENE W4
	void Scene_W4::Initialize()
	{
		m_Camera.origin = { 0.f, 1.f, -5.f };
		m_Camera.updateFovAngle(45.f);

		//Materials
		//const auto matLambert_GreyBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.97f }, 1.f));
		const auto matLambert_GreyBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		//Plane
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GreyBlue);
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GreyBlue);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GreyBlue);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GreyBlue);
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GreyBlue);

		//Triangle (Temp)
		/*auto triangle = Triangle{ { -.75f, .5f, .0f}, {-.75, 2.f, .0f}, {.75, .5f, 0.f} };
		triangle.cullMode = TriangleCullMode::BackFaceCulling;
		triangle.materialIndex = matLambert_White;

		m_Triangles.emplace_back(triangle);*/

		//TriangleMesh
		 
		pMesh = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
		pMesh->positions = { {-.75f, -1.f, .0f}, {-.75f, 1.f, .0f}, {.75f, 1.f, 1.f}, {.75f, -1.f, 0.f} };
		pMesh->indices = {
			0, 1, 2,
			0, 2, 3
		};

		pMesh->CalculateNormals();

		pMesh->Translate({ 0.f, 1.5f, 0.f });
		//pMesh->Scale({ 2.f, 2.f, 2.f });
		pMesh->UpdateAABB();

		pMesh->UpdateTransforms();

		/*pMesh = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
		Utils::ParseOBJ("Resources/simple_cube.obj",
			pMesh->positions,
			pMesh->normals,
			pMesh->indices);

		pMesh->Scale({ 0.7f, 0.7f, 0.7f });
		pMesh->Translate({ 0.0f, 1.f, 0.f });

		pMesh->UpdateAABB();
		pMesh->UpdateTransforms();*/

		/*pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		Utils::ParseOBJ("Resources/Lowpoly_bunny.obj",
			pMesh->positions,
			pMesh->normals,
			pMesh->indices);

		pMesh->Scale({ 2.f, 2.f, 2.f });

		pMesh->UpdateAABB();
		pMesh->UpdateTransforms();*/

		//Light
		AddPointLight({ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, 0.61f, 0.45f });
		AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, 0.8f, 0.45f });
		AddPointLight({ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ 0.34f, 0.47f, 0.68f });

	}
	void Scene_W4::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		pMesh->RotateY(PI_DIV_2 * pTimer->GetTotal());
		pMesh->UpdateTransforms();
	}
#pragma endregion

#pragma region SCENE W4 REFERENCE
	void Scene_W4_ReferenceScene::Initialize()
	{
		sceneName = "Reference Scene";
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.updateFovAngle(45.f);

		//Materials
		const auto matCT_GreyRoughMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.f, 1.f));
		const auto matCT_GreyMediumMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.f, .6f));
		const auto matCT_GreySmoothMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.f, .1f));
		const auto matCT_GreyRoughPlastic = AddMaterial(new Material_CookTorrence({ 0.75f, 0.75f, 0.75f }, 0.0f, 1.f));
		const auto matCT_GreyMediumPlastic = AddMaterial(new Material_CookTorrence({ 0.75f, 0.75f, 0.75f }, 0.0f, .6f));
		const auto matCT_GreySmoothPlastic = AddMaterial(new Material_CookTorrence({ 0.75f, 0.75f, 0.75f }, 0.0f, .1f));

		const auto matLambert_GreyBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		//Plane
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GreyBlue);
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GreyBlue);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GreyBlue);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GreyBlue);
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GreyBlue);

		//Spheres
		AddSphere({ -1.75f, 1.f, 0.f }, 0.75f, matCT_GreyRoughMetal);
		AddSphere({ 0.f, 1.f, 0.f }, 0.75f, matCT_GreyMediumMetal);
		AddSphere({ 1.75f, 1.f, 0.f }, 0.75f, matCT_GreySmoothMetal);
		AddSphere({ -1.75f, 3.f, 0.f }, 0.75f, matCT_GreyRoughPlastic);
		AddSphere({ 0.f, 3.f, 0.f }, 0.75f, matCT_GreyMediumPlastic);
		AddSphere({ 1.75f, 3.f, 0.f }, 0.75f, matCT_GreySmoothPlastic);


		//CW Winding Order!
		const Triangle baseTriangle = { {-.75f, 1.5f, 0.f}, {.75f, 0.f, 0.f}, {-.75, 0.f, 0.f} };

		m_Meshes[0] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		m_Meshes[0]->AppendTriangle(baseTriangle, true);
		m_Meshes[0]->Translate({ -1.75f, 4.5f, 0.0f });
		m_Meshes[0]->UpdateAABB();
		m_Meshes[0]->UpdateTransforms();

		m_Meshes[1] = AddTriangleMesh(TriangleCullMode::FrontFaceCulling, matLambert_White);
		m_Meshes[1]->AppendTriangle(baseTriangle, true);
		m_Meshes[1]->Translate({ 0.f, 4.5f, 0.0f });
		m_Meshes[1]->UpdateAABB();
		m_Meshes[1]->UpdateTransforms();

		m_Meshes[2] = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
		m_Meshes[2]->AppendTriangle(baseTriangle, true);
		m_Meshes[2]->Translate({ 1.75f, 4.5f, 0.0f });
		m_Meshes[2]->UpdateAABB();
		m_Meshes[2]->UpdateTransforms();

		//Light
		AddPointLight({ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, 0.61f, 0.45f });
		AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, 0.8f, 0.45f });
		AddPointLight({ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ 0.34f, 0.47f, 0.68f });

	}
	void Scene_W4_ReferenceScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		//const auto yawAngle = (cos(pTimer->GetTotal()) + 1.f) / 2.f * PI_2;
		for (const auto i : m_Meshes)
		{
			i->RotateY(PI_DIV_2 * pTimer->GetTotal());
			i->UpdateTransforms();
		}

		
	}
#pragma endregion
#pragma region SCENE W4 BUNNY
	void Scene_W4_Bunny::Initialize()
	{
		sceneName = "Bunny Scene";
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.updateFovAngle(45.f);

		//Materials

		const auto matLambert_GreyBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		//Plane
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GreyBlue);
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GreyBlue);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GreyBlue);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GreyBlue);
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GreyBlue);
		
		//Mesh
		pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		Utils::ParseOBJ("Resources/Lowpoly_bunny.obj",
			pMesh->positions,
			pMesh->normals,
			pMesh->indices);

		pMesh->Scale({ 2.f, 2.f, 2.f });

		pMesh->UpdateAABB();
		pMesh->UpdateTransforms();

		//Light
		AddPointLight({ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, 0.61f, 0.45f });
		AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, 0.8f, 0.45f });
		AddPointLight({ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ 0.34f, 0.47f, 0.68f });

	}
	void Scene_W4_Bunny::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		pMesh->RotateY(PI_DIV_2 * pTimer->GetTotal());
		pMesh->UpdateTransforms();

	}
#pragma endregion

}
