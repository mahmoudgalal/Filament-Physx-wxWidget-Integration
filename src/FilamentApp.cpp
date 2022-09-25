#include "FilamentApp.h"
#include <filament/Camera.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filament/TransformManager.h>
#include <filament/LightManager.h>
#include <filament/Renderer.h>
#include <filament/RenderableManager.h>
#include <filament/Scene.h>
#include <filament/Skybox.h>
#include <filament/View.h>
#include<iostream>
#include <utils/EntityManager.h>
#include "MaterialLoader.h"
#include <math/TMatHelpers.h>
#include <math/TVecHelpers.h>

using namespace filament;
using namespace filament::math;
using namespace utils;

using MinFilter = TextureSampler::MinFilter;
using MagFilter = TextureSampler::MagFilter;

FilamentApp& FilamentApp::get() {
	static FilamentApp filamentApp;
	return filamentApp;
}

FilamentApp::FilamentApp() {
	physXManager = std::make_shared<PhysXManager>();
}

void FilamentApp::run(void* windowHandle, void* context, unsigned int w, unsigned int h) {
	mEngine = Engine::create(Engine::Backend::OPENGL, nullptr, context);
	textureLoader = new TextureLoader(mEngine);
	nativeSwapChain = windowHandle;
	mSwapChain = mEngine->createSwapChain(nativeSwapChain);
	mRenderer = mEngine->createRenderer();
	mScene = mEngine->createScene();

	skybox = Skybox::Builder().color({ 0.1, 0.125, 0.25, 1.0 }).build(*mEngine);
	mScene->setSkybox(skybox);

	// Materials
	auto materialLoader = MaterialLoader(mEngine);
	auto materialData = materialLoader.loadMaterialData(MaterialLoader::MaterialType::MaterialTypeUnLit);
	mDefaultMaterial = Material::Builder()
		.package(materialData->getData(), materialData->getSize())
		.build(*mEngine);
	auto sphereMatData = materialLoader.loadMaterialData(MaterialLoader::MaterialType::MaterialTypeLit);
	mLitMaterial = Material::Builder()
		.package(sphereMatData->getData(), sphereMatData->getSize())
		.build(*mEngine);
	auto matTexturedData = materialLoader.loadMaterialData(MaterialLoader::MaterialType::MaterialTypeTextured);
	mTexturedMaterial = Material::Builder()
		.package(matTexturedData->getData(), matTexturedData->getSize())
		.build(*mEngine);

	// Ground
	auto matInstance0 = mDefaultMaterial->createInstance();
	math::float4 gColor{ 0.0f,0.0f,0.0f,0.6f };
	matInstance0->setParameter("baseColor", gColor);
	ground = new ShadowPlane(*mEngine, matInstance0);
	ground->setPosition({ 0, 0, 0 });
	ground->setScale(10, 1, 10);
	mScene->addEntity(ground->getRenderable());



	// Spheres   	
	auto colorComputer = [](int index)->const float3& {
		switch (index)
		{
		case 0:
			return float3{ 0.9f , 0.0f, 0.0f };
		case 1:
			return float3{ 0.0f , 0.0f, 0.9f };
		case 2:
			return float3{ 0.0f , 0.9f, 0.0f };
		case 3:
			return float3{ 0.9f , 0.9f, 0.0f };
		default:
			return float3{ fmax(sin(index % 360),0.4f) ,  fmax(cos(index),0.2f),  fmax(sin(index),0.1f) };
		}
	};

	for (int i = 0; i < 5; i++)
	{
		auto matInstance1 = mLitMaterial->createInstance();
		matInstance1->setParameter("baseColor", RgbType::LINEAR, colorComputer(i));
		matInstance1->setParameter("metallic", 1.0f);
		matInstance1->setParameter("roughness", 0.6f);
		matInstance1->setParameter("reflectance", 0.9f);
		auto item = new Sphere(*mEngine, matInstance1);
		auto name = std::string("Sphere_No_"); name += std::to_string(i);
		item->setName(name);
		rotatingSpheres.push_back(item);
		physXManager->createSphere(0, 3 + 4 * i, 3.4 + i, 1.0f, item);
		auto itemRenderable = item->getRenderable();
		mScene->addEntity(itemRenderable);
		nodes_map[itemRenderable.getId()] = item;
	}

	auto tex = textureLoader->loadTexture("textures/texture.png");
	loadedTextures.push_back(tex);
	// Textured Cube
	{
		auto texCubeMat = mTexturedMaterial->createInstance();
		TextureSampler sampler(MinFilter::LINEAR, MagFilter::LINEAR);
		for (size_t i = 0; i < 4; i++)
		{
			auto matInst = MaterialInstance::duplicate(texCubeMat);
			matInst->setParameter("albedo", tex, sampler);
			auto texturedCube = new TexturedCube(*mEngine, matInst, { 1,0,0 });
			auto name = std::string("Textured_Cube_"); name += std::to_string(i)+" ";
			texturedCube->setName(name);
			texturedCubes.push_back(texturedCube);
			physXManager->createCube(1.0f, i + 10.0f, 2.0f, texturedCube);
			mScene->addEntity(texturedCube->getRenderable());
		}
		mEngine->destroy(texCubeMat);
	}

	// Walls
	{
		auto texCubeMat = mTexturedMaterial->createInstance();
		TextureSampler sampler(MinFilter::LINEAR, MagFilter::LINEAR);
		for (size_t i = 0; i < 4; i++)
		{
			auto matInst = MaterialInstance::duplicate(texCubeMat);
			matInst->setParameter("albedo", tex, sampler);
			auto texturedWall = new TexturedCube(*mEngine, matInst, { 1,0,0 });
			auto name = std::string("Wall_Cube_"); name += std::to_string(i) ;
			texturedWall->setName(name);
			walls.push_back(texturedWall);
			auto scale = PxVec3(40, 40, 40);
			float3 s = { 40 };
			texturedWall->setObjScale(s);
			texturedWall->setCastShadows(false);
			auto x = i % 2 ? 40 * 2 : 0; auto y = 40; auto z = i % 2 == 0 ? 40 * 2 : 0;
			if (i == 1) x = -x; if (i == 2) z = -z;
			physXManager->createStaticCube(PxVec3(x, y, z),
				scale, texturedWall);
			mScene->addEntity(texturedWall->getRenderable());
		}
		mEngine->destroy(texCubeMat);
	}

	// Light
	light = EntityManager::get().create();
	filament::LightManager::Builder(filament::LightManager::Type::SUN)
		.color(Color::toLinear<ACCURATE>(sRGBColor(0.98f, 0.92f, 0.89f)))
		.intensity(110000)
		.direction({ 0.5, -1, 0.9 })
		.sunAngularRadius(1.9f)
		.sunHaloSize(100)
		.castShadows(true)
		.build(*mEngine, light);
	mScene->addEntity(light);

	// create cameras
	const uint32_t width = w;
	const uint32_t height = h;
	const float aspect = (float)width / height;
	mMainCamera = mEngine->createCamera(utils::EntityManager::get().create());
	mMainCamera->setExposure(16.0f, 1 / 125.0f, 100.0f);
	double near = 0.1;
	double far = 200;
	mMainCamera->setLensProjection(mCameraFocalLength, double(w) / h, near, far);
	mMainCamera->lookAt({ -1, 0, -12 }, { 0, 0, -4 }, { 0, 1, 0 });

	// Camera Manipulators
	mMainCameraMan = CameraManipulator::Builder()
		.targetPosition(0, 0, -4)
		.orbitHomePosition(0, 19, -26)
		.upVector(0, 1, 0)
		.flightMoveDamping(15.0)
		.build(camutils::Mode::ORBIT);

	rotatingSpheres[0]->setPosition({ 0, -4, -4 });
	rotatingSpheres[1]->setPosition({ 0, 4, -4 });
	rotatingSpheres[2]->setPosition({ 0, 0, -8 });

	// Create the View
	view = mEngine->createView();
	view->setName("Main View");
	view->setScene(mScene);
	view->setPostProcessingEnabled(false);
	view->setViewport({ 0, 0, w, h });
	view->setShadowingEnabled(true);
	view->setAmbientOcclusionOptions({ .enabled = true });
	mMainCameraMan->setViewport(w, h);
	view->setCamera(mMainCamera);
}

void FilamentApp::render(long long time) {
	if (mRenderer->beginFrame(mSwapChain)) {
		mTimeStp = time - mLastTime;
		physXManager->simulate(mTimeStp / 1000.0f);
		mRenderer->render(view);
		auto eng = mEngine;
		auto angle = (time / 1000.0) * 60.0 * (2 * math::F_PI) / 360;

		//cube->rotateY(-4 * angle);		

		mMainCameraMan->update(mTimeStp / 1000.0f);
		float3 eye, center, up;
		mMainCameraMan->getLookAt(&eye, &center, &up);

		mMainCamera->lookAt(eye, center, up);

		// Physx
		physXManager->updateNodes();

		mLastTime = time;
		mRenderer->endFrame();
	}
}

void FilamentApp::onMouseDown(int x, int y, const MouseButton& mbtn)
{
	if (mMainCameraMan && mbtn == Left_Button) {
		mMainCameraMan->grabBegin(x, y, false);
	}
	if (mbtn == Right_Button) {
		auto projectionMat = mMainCamera->getProjectionMatrix();
		auto viewMat = mMainCamera->getViewMatrix();
		auto resultMat = projectionMat*viewMat;
		auto inversed = math::details::matrix::inverse(resultMat);
		auto screenWidth = view->getViewport().width;
		auto screenHeight = view->getViewport().height;
		math::float4 lRayStart_NDC(
			((float)x / (float)screenWidth - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
			((float)((screenHeight-y) / (float)screenHeight) - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
			-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
			1.0f
		);
		math::float4 lRayEnd_NDC(
			((float)x / (float)screenWidth - 0.5f) * 2.0f,
			((float)((screenHeight-y) / (float)screenHeight) - 0.5f) * 2.0f,
			0.0,
			1.0f
		);
		math::float4 lRayStart_world = inversed * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
		math::float4 lRayEnd_world   = inversed * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;

		math::float4 lRayDir_world(lRayEnd_world - lRayStart_world);
		//normalize( lRayDir_world);
		auto value = sqrtf(powf(lRayDir_world.x, 2) + powf(lRayDir_world.y, 2) + powf(lRayDir_world.z, 2));
		lRayDir_world /= value;
		
		 auto origin = physx::PxVec3(lRayStart_world.x, lRayStart_world.y, lRayStart_world.z);
		 auto dir = physx::PxVec3(lRayDir_world.x, lRayDir_world.y, lRayDir_world.z);
		physXManager->rayCast(origin,dir);
	}
}

void FilamentApp::onMouseMove(int x, int y)
{
	if (mMainCameraMan) {
		mMainCameraMan->grabUpdate(x, y);
	}
}

void FilamentApp::onMouseUp(int x, int y, const MouseButton& mbtn)
{
	if (mMainCameraMan && mbtn == Left_Button) {
		mMainCameraMan->grabEnd();
	}
}

void FilamentApp::createEyeProjectile(const ProjectileType& t)
{
	auto camPos = mMainCamera->getPosition();
	auto eye = mMainCamera->getForwardVector();

	Node* s = nullptr;
	if (t == SphereProjectile) {
		auto matInstance1 = mLitMaterial->createInstance();
		matInstance1->setParameter("baseColor", RgbType::LINEAR, { 0.8f });
		matInstance1->setParameter("metallic", 1.0f);
		matInstance1->setParameter("roughness", 0.6f);
		matInstance1->setParameter("reflectance", 0.9f);
		s = new Sphere(*mEngine, matInstance1);
		auto name = std::string("Sphere_Projectile");
		s->setName(name);
		physXManager->createSphere(camPos.x, camPos.y, camPos.z, 1.0f, s, PxVec3(eye.x, eye.y, eye.z) * 100);
	}
	else {
		auto texCubeMat = mTexturedMaterial->createInstance();
		TextureSampler sampler(MinFilter::LINEAR, MagFilter::LINEAR);
		texCubeMat->setParameter("albedo", loadedTextures[0], sampler);
		s = new TexturedCube(*mEngine, texCubeMat, { 1,0,0 });
		auto name = std::string("Cube_Projectile");
		s->setName(name);
		physXManager->createCube(camPos.x, camPos.y, camPos.z, s, PxVec3(eye.x, eye.y, eye.z) * 100);
	}
	projectiles.push_back(s);
	mScene->addEntity(s->getRenderable());
}

void FilamentApp::createPyramidStack(uint32_t size, float centerX, float centerY, float centerZ)
{
	for (size_t i = 0; i < size; i++)//y
	{
		for (size_t j = i; j < size; j++)//x
		{
			for (size_t k = i; k < size; k++)//z
			{
				auto texCubeMat = mTexturedMaterial->createInstance();
				TextureSampler sampler(MinFilter::LINEAR, MagFilter::LINEAR);
				texCubeMat->setParameter("albedo", loadedTextures[0], sampler);
				auto s = new TexturedCube(*mEngine, texCubeMat, { 1,0,0 });
				auto name = std::string("Textured_Cube_"); name += std::to_string(i + j + k);
				s->setName(name);
				physXManager->createCube(centerX + (2 * j) - i, centerY + 1 + 2 * i, centerZ + (2 * k) - i, s);
				pyramids.push_back(s);				
				mScene->addEntity(s->getRenderable());
			}
		}

	}
}

FilamentApp::~FilamentApp() {
	mEngine->destroyCameraComponent(mMainCamera->getEntity());
	delete textureLoader;
	delete ground;

	for (auto item : rotatingSpheres) {
		delete item;
	}
	rotatingSpheres.clear();

	for (auto item : texturedCubes) {
		delete item;
	}
	texturedCubes.clear();

	for (auto item : projectiles) {
		delete item;
	}
	projectiles.clear();

	for (auto item : pyramids) {
		delete item;
	}
	pyramids.clear();

	for (auto item : walls) {
		delete item;
	}
	walls.clear();

	for (auto item : loadedTextures) {
		mEngine->destroy(item);
	}
	loadedTextures.clear();

	delete mMainCameraMan;

	mEngine->destroy(light);
	mEngine->destroy(mDefaultMaterial);
	mEngine->destroy(mTexturedMaterial);
	mEngine->destroy(mLitMaterial);
	mEngine->destroy(skybox);
	mEngine->destroy(view);
	mEngine->destroy(mRenderer);
	mEngine->destroy(mSwapChain);
	mEngine->destroy(mScene);
	Engine::destroy(&mEngine);
	mEngine = nullptr;
}



