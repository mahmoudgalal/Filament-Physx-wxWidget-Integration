#pragma once

#include <vector>
#include <math/vec3.h>
#include <math/mat4.h>
#include <filament/Engine.h>
#include <filament/Box.h>
#include <filament/Camera.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <utils/Entity.h>
#include "Node.h"


class TexturedCube : public Node
{
public:
    enum RenderMode
    {
        RENDER_MODE_SOLID,
        RENDERMODE_WIREFRAME
    } renderMode;

    TexturedCube(filament::Engine& engine, filament::MaterialInstance* material,
        filament::math::float3 linearColor,
        bool culling = true);

    utils::Entity getSolidRenderable() {
        return mSolidRenderable;
    }

    virtual utils::Entity& getRenderable() {
        return mSolidRenderable;
    }

    virtual filament::Engine& getEngine() {
        return mEngine;
    }

    void rotateX(double angle);
    void rotateY(double angle);
    void rotateZ(double angle);
    void rotateXYZ(double angle);
    void setScale(float s);
    void setScale(float sx, float sy, float sz);
    void setPosition(filament::math::float3 const& position);
    ~TexturedCube();

private:
    static constexpr size_t WIREFRAME_OFFSET = 3 * 2 * 6;
    static const uint32_t mIndices[];
    static const filament::math::float3 mVertices[];

    filament::Engine& mEngine;
    filament::VertexBuffer* mVertexBuffer = nullptr;
    filament::IndexBuffer* mIndexBuffer = nullptr;
    filament::Material const* mMaterial = nullptr;
    filament::MaterialInstance* mMaterialInstanceSolid = nullptr;
    filament::MaterialInstance* mMaterialInstanceWireFrame = nullptr;
    utils::Entity mSolidRenderable;
    utils::Entity mWireFrameRenderable;
    filament::math::mat4f translation;
    filament::math::mat4f rotation;
    filament::math::mat4f scale;
};

