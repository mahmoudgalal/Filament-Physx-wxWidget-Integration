
#include "MaterialLoader.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

using namespace filamat;
using namespace filament;

MaterialLoader::MaterialLoader(Engine* engine) :mEngine(engine)
{
	// Must be called before any materials can be built.
	MaterialBuilder::init();
	typeNamesMap[MaterialType::MaterialTypeLit] = "lit_material";
	typeNamesMap[MaterialType::MaterialTypeUnLit] = "color_material";
	typeNamesMap[MaterialType::MaterialTypeTextured] = "texture_material";
}

std::shared_ptr<filamat::Package> MaterialLoader::loadMaterialData(MaterialType materialType)
{
	std::string filename = MATERIALS_FOLDER;
	filename += "\/";
	filename += typeNamesMap[materialType];
	filename += ".data";
	if (std::filesystem::exists(filename)) {
		auto size = std::filesystem::file_size(filename);
		std::ifstream istrm(filename, std::ios::binary);
		char* data = new char[size];
		istrm.read(reinterpret_cast<char*>(data), size);
		return std::make_shared<Package>(data, size);
	}
	std::ofstream materialStream(filename, std::ios::binary);
	MaterialBuilder builder;
	builder.name(typeNamesMap[materialType]);

	if (materialType == MaterialType::MaterialTypeTextured) {
		builder.require(VertexAttribute::UV0)
			//.parameter(UniformType::FLOAT4, ParameterPrecision::DEFAULT, "baseColor")
			.blending(BlendingMode::OPAQUE)
			.parameter(SamplerType::SAMPLER_2D, "albedo")
			.material("void material (inout MaterialInputs material) {"
				"  prepareMaterial(material);"
				"  material.baseColor.rgb = texture(materialParams_albedo, getUV0()).rgb;"
				"}")
			.shading(MaterialBuilder::Shading::UNLIT);

	}
	else if (materialType == MaterialType::MaterialTypeUnLit) {
		builder
			.blending(BlendingMode::TRANSPARENT)
			.culling(MaterialBuilder::CullingMode::NONE)
			.depthCulling(true)
			.shadowMultiplier(true)
			.parameter(UniformType::FLOAT4, ParameterPrecision::DEFAULT, "baseColor")
			.material("void material (inout MaterialInputs material) {"
				"  prepareMaterial(material);"
				"  material.baseColor = materialParams.baseColor;"
				"  } ")
			.shading(MaterialBuilder::Shading::UNLIT);
	}
	else if (materialType == MaterialType::MaterialTypeLit)
	{
		builder
			.parameter(UniformType::FLOAT3, ParameterPrecision::DEFAULT, "baseColor")
			.parameter(UniformType::FLOAT, ParameterPrecision::DEFAULT, "metallic")
			.parameter(UniformType::FLOAT, ParameterPrecision::DEFAULT, "roughness")
			.parameter(UniformType::FLOAT, ParameterPrecision::DEFAULT, "reflectance")
			.material("void material (inout MaterialInputs material) {"
				"  prepareMaterial(material);"
				"  material.baseColor.rgb = materialParams.baseColor;"
				"  material.metallic = materialParams.metallic;"
				"  material.roughness = materialParams.roughness;"
				"  material.reflectance = materialParams.reflectance;"
				"  } ")
			.shading(MaterialBuilder::Shading::LIT);
	}
	builder.targetApi(MaterialBuilder::TargetApi::OPENGL)
		.platform(MaterialBuilder::Platform::DESKTOP);

	auto& jobSys = mEngine->getJobSystem();
	Package package = builder.build(jobSys);
	if (package.isValid()) {
		std::cout << "Material build success!" << std::endl;
	}
	materialStream.write(reinterpret_cast<char*>(package.getData()), package.getSize());
	return std::make_shared<Package>(package.getData(), package.getSize());
}

MaterialLoader::~MaterialLoader()
{
	// Call when finished building all materials to release internal MaterialBuilder resources.
	MaterialBuilder::shutdown();
}

