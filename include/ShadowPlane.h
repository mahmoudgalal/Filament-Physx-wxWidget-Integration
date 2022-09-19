#pragma once
#include <vector>
#include <math/vec3.h>
#include <math/mat4.h>
#include <filament/Engine.h>
#include <filament/Box.h>
#include <filament/Camera.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filament/IndexBuffer.h>
#include <filament/VertexBuffer.h>
#include <utils/Entity.h>
#include "Node.h"

class ShadowPlane:public Node
{
public:
    ShadowPlane(filament::Engine& engine, filament::MaterialInstance* materialInstance);

    virtual utils::Entity& getRenderable() {
        return mRenderable;
    }

    virtual filament::Engine& getEngine() {
        return mEngine;
    }
    void rotateX(double angle);
    void rotateY(double angle);
    void rotateZ(double angle);
    void rotateXYZ(double angle);
    void setPosition(filament::math::float3 const& position);
    void setScale(float s);
    void setScale(float sx, float sy, float sz);
    ~ShadowPlane();

private:
    filament::Engine& mEngine;
    filament::VertexBuffer* mVertexBuffer = nullptr;
    filament::IndexBuffer* mIndexBuffer = nullptr;
    filament::MaterialInstance* mMaterialInstance = nullptr;
    utils::Entity mRenderable;
    filament::math::mat4f translation;
    filament::math::mat4f rotation;
    filament::math::mat4f scale;
};

