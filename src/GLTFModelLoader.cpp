#pragma warning(disable : 4146)

#include "GLTFModelLoader.h"
#include<fstream>
#include<iostream>
#include <filesystem>
#include "materials/uberarchive.h"

void GLTFModelLoader::updateLoad()
{
	if (resourceLoader && resourceLoader->asyncGetLoadProgress() < 1.0f)
		resourceLoader->asyncUpdateLoad();
}

GLTFModelLoader::GLTFModelLoader(Engine* engine) :mEngine(engine)
{
	stbDecoder = createStbProvider(engine);
	ktxDecoder = createKtx2Provider(engine);
	configuration.engine = engine;
	configuration.normalizeSkinningWeights = true;

	materials = //createUbershaderProvider(engine, UBERARCHIVE_DEFAULT_DATA, UBERARCHIVE_DEFAULT_SIZE);
		createJitShaderProvider(engine);
	names = new NameComponentManager(EntityManager::get());
	assetLoader = AssetLoader::create({ engine, materials,names });
}

GltfModel* GLTFModelLoader::loadAsset(const std::string& filename)
{
	if (!std::filesystem::exists(filename)) {
		std::cout << "gltf File: " << filename << " Not Found" << std::endl;
		return nullptr;
	}
	// Consume the glTF file.
	auto contentSize = std::filesystem::file_size(filename);
	auto isBin = filename.ends_with(".glb");
	std::ifstream in(filename.c_str(), std::ifstream::binary | std::ifstream::in);
	std::vector<uint8_t> buffer(static_cast<unsigned long>(contentSize));
	if (!in.read((char*)buffer.data(), contentSize)) {
		std::cout << "Unable to read " << filename << std::endl;
		return nullptr;
	}

	// Parse the glTF file and create Filament entities.
	FilamentAsset* asset = isBin ? assetLoader->createAssetFromBinary(buffer.data(), buffer.size())
		: assetLoader->createAssetFromJson(buffer.data(), buffer.size());
	buffer.clear();
	buffer.shrink_to_fit();

	if (!asset) {
		std::cout << "Unable to parse " << filename << std::endl;
		return nullptr;
	}
	std::filesystem::path p(filename);
	const std::string dir = p.string();
	ResourceConfiguration configuration = {};
	configuration.gltfPath = dir.c_str();
	configuration.engine = mEngine;
	configuration.recomputeBoundingBoxes = true;
	configuration.ignoreBindTransform = false;
	configuration.normalizeSkinningWeights = true;
	if (resourceLoader)
		delete resourceLoader;
	resourceLoader = new ResourceLoader(configuration);
	resourceLoader->addTextureProvider("image/png", stbDecoder);
	resourceLoader->addTextureProvider("image/jpeg", stbDecoder);
	resourceLoader->addTextureProvider("image/ktx2", ktxDecoder);
	if (!resourceLoader->loadResources(asset)) {
		std::cout << "Unable to start loading resources for " << filename << std::endl;
		std::cout << "Unable to start resources from " << dir << std::endl;
		return nullptr;
	}
	std::cout << "gltf file resources loaded from " << dir.c_str() << std::endl;
	std::cout << "gltf File: " << filename << " Loaded correctly!" << std::endl;
	asset->releaseSourceData();
	return new GltfModel(asset);
}

void GLTFModelLoader::destroyAsset(GltfModel* asset)
{
	assetLoader->destroyAsset(asset->asset);
}

GLTFModelLoader::~GLTFModelLoader()
{
	//resourceLoader->asyncCancelLoad();
	materials->destroyMaterials();
	delete materials;
	delete names;
	if (resourceLoader)
		delete resourceLoader;
	delete stbDecoder;
	delete ktxDecoder;
	AssetLoader::destroy(&assetLoader);
}


