#pragma once

#pragma warning(disable : 4146)

#include <filament/Engine.h>
#include <filament/Texture.h>
#include <filament/TextureSampler.h>

class TextureLoader
{
public:
	TextureLoader(filament::Engine* engine);
	filament::Texture* loadTexture(const char* path);
	~TextureLoader();

private:
	filament::Engine* mEngine = nullptr;
};

