

#include "TextureLoader.h"
#include <stb_image.h>
#include <iostream>

using namespace filament;

TextureLoader::TextureLoader(filament::Engine* engine) :mEngine(engine) {

}
Texture* TextureLoader::loadTexture(const char* path) {
	std::cout << "Loading texture: " << path << std::endl;
	int w, h, n;
	unsigned char* data = stbi_load(path, &w, &h, &n, 4);
	if (data == nullptr) {
		std::cerr << "The texture: " << path << " could not be loaded" << std::endl;
		return nullptr;
	}
	std::cout << "Loaded texture: " << w << "x" << h << std::endl;
	Texture::PixelBufferDescriptor buffer(data, size_t(w * h * 4),
		Texture::Format::RGBA, Texture::Type::UBYTE,
		(Texture::PixelBufferDescriptor::Callback)&stbi_image_free);
	auto tex = Texture::Builder()
		.width(uint32_t(w))
		.height(uint32_t(h))
		.levels(1)
		.sampler(Texture::Sampler::SAMPLER_2D)
		.format(Texture::InternalFormat::RGBA8)
		.build(*mEngine);
	tex->setImage(*mEngine, 0, std::move(buffer));
	return tex;
}

TextureLoader::~TextureLoader()
{
}


