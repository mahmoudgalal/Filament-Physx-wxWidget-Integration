#pragma warning(disable : 4146)
#include "GltfModel.h"

GltfModel::GltfModel(FilamentAsset* asset)
{
	this->asset = asset;
}

void GltfModel::addToScene(Scene* scene)
{
	auto& rcm = getEngine().getRenderableManager();
	scene->addEntities(asset->getLightEntities(), asset->getLightEntityCount());
	for (size_t i = 0, n = asset->getRenderableEntityCount(); i < n; i++) {
		auto ri = rcm.getInstance(asset->getRenderableEntities()[i]);
		rcm.setScreenSpaceContactShadows(ri, true);
	}
	scene->addEntities(asset->getRenderableEntities(), asset->getRenderableEntityCount());
	//scene->addEntity(getWireframe());
}

GltfModel::~GltfModel()
{

}

