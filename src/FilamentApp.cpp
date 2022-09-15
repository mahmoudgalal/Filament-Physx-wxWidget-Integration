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

	auto materialLoader = MaterialLoader(mEngine);
	auto materialData = materialLoader.loadMaterialData(MaterialLoader::MaterialType::MaterialTypeUnLit);
	mDefaultMaterial = Material::Builder()
		.package(materialData->getData(), materialData->getSize())
		.build(*mEngine);

	//Cube
	math::float4 cubeColor{ 1.0f,0.0f,0.0f,0.5f };
	auto mat = mDefaultMaterial->createInstance();
	mat->setParameter("baseColor", cubeColor);
	cube = new Cube(*mEngine, mat, { 1,0,0 }, true, Cube::RenderMode::RENDER_MODE_SOLID);
	mScene->addEntity(cube->getRenderable());

	// Spheres   
	math::float4 color{ 0.69f, 0.69f, 0.0f,0.5f };
	auto sphereMatData = materialLoader.loadMaterialData(MaterialLoader::MaterialType::MaterialTypeLit);
	mLitMaterial = Material::Builder()
		.package(sphereMatData->getData(), sphereMatData->getSize())
		.build(*mEngine);
	auto sphereMat = mLitMaterial->createInstance();
	sphereMat->setParameter("baseColor", RgbType::LINEAR, float3{ 0.8 });
	sphereMat->setParameter("metallic", 1.0f);
	sphereMat->setParameter("roughness", 0.6f);
	sphereMat->setParameter("reflectance", 0.9f);

	sphere = new Sphere(*mEngine, sphereMat);
	mScene->addEntity(sphere->getSolidRenderable());

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
			return float3{ 0.9f , 0.9f, 0.9f };;
		}
	};

	for (int i = 0; i < 4; i++)
	{
		auto matInstance1 = MaterialInstance::duplicate(sphereMat);
		matInstance1->setParameter("baseColor", RgbType::LINEAR, colorComputer(i));
		matInstance1->setParameter("metallic", 1.0f);
		matInstance1->setParameter("roughness", 0.6f);
		matInstance1->setParameter("reflectance", 0.9f);
		rotatingSpheres.push_back(new Sphere(*mEngine, matInstance1));
		mScene->addEntity(rotatingSpheres[i]->getSolidRenderable());
	}

	// Textured Cube
	math::float4 texCubeColor{ 0.0f, 0.69f, 0.0f,0.5f };
	auto matTexturedData = materialLoader.loadMaterialData(MaterialLoader::MaterialType::MaterialTypeTextured);
	mTexturedMaterial = Material::Builder()
		.package(matTexturedData->getData(), matTexturedData->getSize())
		.build(*mEngine);
	auto texCubeMat = mTexturedMaterial->createInstance();
	auto tex = textureLoader->loadTexture("textures/texture.png");
	loadedTextures.push_back(tex);
	TextureSampler sampler(MinFilter::LINEAR, MagFilter::LINEAR);
	texCubeMat->setParameter("albedo", tex, sampler);
	texturedCube = new TexturedCube(*mEngine, texCubeMat, { 1,0,0 });
	mScene->addEntity(texturedCube->getRenderable());

	// Light
	light = EntityManager::get().create();
	filament::LightManager::Builder(filament::LightManager::Type::SUN)
		.color(Color::toLinear<ACCURATE>(sRGBColor(0.98f, 0.92f, 0.89f)))
		.intensity(110000)
		.direction({ 0.5, -1, 0.9 })
		.sunAngularRadius(120.0f)
		.sunHaloSize(100)
		.castShadows(false)
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
		.orbitHomePosition(0, 0, -26)
		.upVector(0, 1, 0)
		.flightMoveDamping(15.0)
		.build(camutils::Mode::ORBIT);

	cube->setPosition({ 4, 0, -4 });
	sphere->setPosition({ 0, 0, -4 });
	texturedCube->setPosition({ -4, 0, -4 });
	rotatingSpheres[0]->setPosition({ 0, -4, -4 });
	rotatingSpheres[1]->setPosition({ 0, 4, -4 });
	rotatingSpheres[2]->setPosition({ 0, 0, -8 });

	// Create the View
	view = mEngine->createView();
	view->setName("Main View");
	view->setScene(mScene);
	view->setPostProcessingEnabled(false);
	view->setViewport({ 0, 0, w, h });
	mMainCameraMan->setViewport(w, h);

	view->setCamera(mMainCamera);
}

void FilamentApp::render(long long time) {
	if (mRenderer->beginFrame(mSwapChain)) {
		mTimeStp = time - mLastTime;
		mRenderer->render(view);
		auto eng = mEngine;
		auto angle = (time / 1000.0) * 60.0 * (2 * math::F_PI) / 360;
		
		cube->rotateY(-4 * angle);		
		sphere->rotateY(angle);
		texturedCube->rotateY(angle);
		// Animating Balls Rotation
		// Lemniscate of Bernoulli rotation Curve
		rotatingSpheres[0]->setPosition({ 8 * cos(angle + 1.25f) / (1 + powf(sin(angle + 1.25f),2.0f)), -4 ,
		   -4 + (8 * sin(angle + 1.25f) * cos(angle + 1.25f) / ((1 + powf(sin(angle + 1.25f),2.0f)))) });
		rotatingSpheres[1]->setPosition({ 8 * cos(angle) / (1 + powf(sin(angle),2.0f)), 4 ,
		  -4 + (8 * sin(angle) * cos(angle) / ((1 + powf(sin(angle),2.0f)))) });
		// Circle rotation
		rotatingSpheres[2]->setPosition({ 8 * cos(angle) , 0 ,
		  -4 + 8 * sin(angle) });
		rotatingSpheres[3]->setPosition({ 0 , 8 * sin(angle) ,
		  -4 + 8 * cos(angle) });
		
		mMainCameraMan->update(mTimeStp / 1000.0f);
		float3 eye, center, up;
		mMainCameraMan->getLookAt(&eye, &center, &up);

		mMainCamera->lookAt(eye, center, up);
		mLastTime = time;
		mRenderer->endFrame();
	}
}

void FilamentApp::onMouseDown(int x, int y)
{
	if (mMainCameraMan) {
		mMainCameraMan->grabBegin(x, y, false);
	}
}

void FilamentApp::onMouseMove(int x, int y)
{
	if (mMainCameraMan) {
		mMainCameraMan->grabUpdate(x, y);
	}
}

void FilamentApp::onMouseUp(int x, int y)
{
	if (mMainCameraMan) {
		mMainCameraMan->grabEnd();
	}
}

FilamentApp::~FilamentApp() {
	mEngine->destroyCameraComponent(mMainCamera->getEntity());
	delete textureLoader;
	delete cube;
	delete sphere;
	for (auto item : rotatingSpheres) {
		delete item;
	}
	rotatingSpheres.clear();
	delete texturedCube;
	delete mMainCameraMan;
	for (auto item : loadedTextures) {
		mEngine->destroy(item);
	}
	loadedTextures.clear();
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




