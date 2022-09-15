#pragma once
#include <filamat/MaterialBuilder.h>
#include <filament/Engine.h>
#include <filamat/Enums.h>
#include <map>
#include<memory>

class MaterialLoader
{
public:
	MaterialLoader(filament::Engine* engine);

	enum MaterialType
	{
		MaterialTypeLit,
		MaterialTypeUnLit,
		MaterialTypeTextured
	};
	std::shared_ptr<filamat::Package> loadMaterialData(MaterialType type = MaterialType::MaterialTypeUnLit);
	~MaterialLoader();
private:
	filament::Engine* mEngine = nullptr;
	std::map< MaterialType, const char*> typeNamesMap;
	const char* MATERIALS_FOLDER = "materials";
};

