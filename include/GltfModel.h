#pragma once
#include"Node.h"
#include"GLTFModelLoader.h"

#include <gltfio/FilamentAsset.h>
#include <filament/Scene.h>


using namespace filament;
using namespace filament::gltfio;

class GltfModel: public Node
{
private:
	friend class GLTFModelLoader;
	FilamentAsset* asset = nullptr;

public:
	GltfModel(FilamentAsset* asset);
	void addToScene(Scene* scene);
	Engine& getEngine() override {
		return *(asset->getEngine());
	}
	utils::Entity& getRenderable() override {
		auto ret = asset->getRoot();
		return ret;
	}
	utils::Entity const* getEntities() const noexcept {		
		return asset->getEntities();
	}
	utils::Entity getWireframe() {
		auto ret = asset->getWireframe();
		return ret;
	}
	const filament::Aabb getBoundingBox() {		
		 return asset->getBoundingBox();
	}

	size_t getEntityCount() const noexcept {
		return asset->getEntityCount();
	}

	virtual ~GltfModel();
	//const FilamentAsset* getAsset() { return asset; };
};

