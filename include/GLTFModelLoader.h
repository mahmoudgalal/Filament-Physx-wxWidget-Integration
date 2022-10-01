#pragma once

#include <gltfio/AssetLoader.h>
#include <gltfio/FilamentAsset.h>
#include <gltfio/ResourceLoader.h>
#include <gltfio/TextureProvider.h>
#include <string>
#include "GltfModel.h"
#include <utils/NameComponentManager.h>

using namespace filament;
using namespace filament::gltfio;
using namespace utils;

class GltfModel;

class GLTFModelLoader
{
	Engine* mEngine = nullptr;
	gltfio::AssetLoader* assetLoader = nullptr;
	gltfio::ResourceLoader* resourceLoader = nullptr;
	gltfio::TextureProvider* stbDecoder = nullptr;
	gltfio::TextureProvider* ktxDecoder = nullptr;
	NameComponentManager* names;
	MaterialProvider* materials = nullptr;
	ResourceConfiguration configuration = {};

public:
	GLTFModelLoader(Engine* engine);
	void updateLoad();
	GltfModel* loadAsset(const std::string& path);
	void destroyAsset(GltfModel* asset);
	~GLTFModelLoader();
};

