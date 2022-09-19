#pragma once

#pragma warning(disable : 4146)

#include <filament/Engine.h>
#include <filament/Viewport.h>
#include <filament/IndexBuffer.h>
#include <filament/VertexBuffer.h>
#include <camutils/Manipulator.h>
#include <utils/Entity.h>
#include <memory>
#include <filament/Skybox.h>
#include "Cube.h"
#include "Sphere.h"
#include "TextureLoader.h"
#include "TexturedCube.h"
#include "PhysXManager.h"
#include "ShadowPlane.h"
#include<vector>

namespace filament {
	class Renderer;
	class Scene;
	class View;
} // namespace filament

class FilamentApp
{
	FilamentApp();
	using CameraManipulator = filament::camutils::Manipulator<float>;
	filament::Engine* mEngine = nullptr;
	filament::Viewport mViewport;
	filament::View* view = nullptr;
	filament::SwapChain* mSwapChain = nullptr;
	filament::Renderer* mRenderer = nullptr;
	FilamentApp* const mFilamentApp = nullptr;
	filament::Scene* mScene = nullptr;
	filament::Camera* mMainCamera;
	filament::Material const* mDefaultMaterial = nullptr;
	filament::Material const* mTexturedMaterial = nullptr;
	filament::Material const* mLitMaterial = nullptr;
	void* nativeSwapChain = nullptr;
	float mCameraFocalLength = 28.0f;
	filament::Skybox* skybox = nullptr;
	//Cube* cube = nullptr;
	ShadowPlane* ground = nullptr;
	std::vector < TexturedCube*> texturedCubes;
	//Sphere* sphere = nullptr;
	std::vector<Sphere*> rotatingSpheres;
	std::vector<Node*> projectiles;
	std::vector<Node*> pyramids;
	std::vector<Node*> walls;
	TextureLoader* textureLoader = nullptr;
	utils::Entity light;
	std::vector<filament::Texture*> loadedTextures;
	CameraManipulator* mMainCameraMan = nullptr;
	long long mLastTime = 0, mTimeStp = 0;
	std::shared_ptr<PhysXManager> physXManager;
	 

public:
	enum MaterialType
	{
		MaterialTypeLit,
		MaterialTypeUnLit,
		MaterialTypeTextured
	};
	enum ProjectileType {
		CubeProjectile,
		SphereProjectile
	};
	static FilamentApp& get();
	void run(void* windowHandle, void* context, unsigned int w = 0, unsigned int h = 0);
	void render(long long time = 0);
	void onMouseDown(int x, int y);
	void onMouseMove(int x, int y);
	void onMouseUp(int x, int y);
	void createEyeProjectile(const ProjectileType& t = ProjectileType::SphereProjectile);
	void createPyramidStack(uint32_t size, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f);

	~FilamentApp();
};

