#pragma once
#include <utils/Entity.h>
#include <math/vec3.h>
#include <math/mat4.h>

namespace filament {
    class Engine;
    class IndexBuffer;
    class Material;
    class MaterialInstance;
    class VertexBuffer;
}

class Sphere {
public:

    Sphere(filament::Engine& engine,
        filament::MaterialInstance * materialInstance,
        bool culling = true);
    ~Sphere();

    Sphere(Sphere const&) = delete;
    Sphere& operator = (Sphere const&) = delete;

    Sphere(Sphere&& rhs) noexcept
        : mEngine(rhs.mEngine),
        mMaterialInstance(rhs.mMaterialInstance),
        mRenderable(rhs.mRenderable){
        rhs.mMaterialInstance = {};
        rhs.mRenderable = {};
    }

    utils::Entity getSolidRenderable() const {
        return mRenderable;
    }

    filament::MaterialInstance* getMaterialInstance() {
        return mMaterialInstance;
    }

    Sphere& setPosition(filament::math::float3 const& position) noexcept;
    Sphere& setRadius(float radius) noexcept;
    void rotateX(double angle);
    void rotateY(double angle);
    void rotateZ(double angle);

private:
    filament::Engine& mEngine;
    filament::MaterialInstance* mMaterialInstance = nullptr;
    utils::Entity mRenderable;
    filament::math::mat4f translation;
    filament::math::mat4f rotation;
    filament::math::mat4f scale;

};

