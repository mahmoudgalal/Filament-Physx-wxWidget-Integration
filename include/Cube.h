/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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

class Cube: public Node {
public:
    enum RenderMode
    {
        RENDER_MODE_SOLID,
        RENDERMODE_WIREFRAME
    } renderMode;

    Cube(filament::Engine& engine, filament::MaterialInstance* materialInstance,
        filament::math::float3 linearColor,
        bool culling = true, RenderMode mode = RENDERMODE_WIREFRAME);

    utils::Entity getSolidRenderable() {
        return mSolidRenderable;
    }

    utils::Entity getWireFrameRenderable() {
        return mWireFrameRenderable;
    }

    virtual utils::Entity& getRenderable() {
        return renderMode == RENDERMODE_WIREFRAME? mWireFrameRenderable: mSolidRenderable;
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
    ~Cube();   

private:
    static constexpr size_t WIREFRAME_OFFSET = 3 * 2 * 6;
    static const uint32_t mIndices[];
    static const filament::math::float3 mVertices[];

    filament::Engine& mEngine;
    filament::VertexBuffer* mVertexBuffer = nullptr;
    filament::IndexBuffer* mIndexBuffer = nullptr;
    filament::MaterialInstance* mMaterialInstance = nullptr;
    utils::Entity mSolidRenderable;
    utils::Entity mWireFrameRenderable;
    filament::math::mat4f translation;
    filament::math::mat4f rotation;
    filament::math::mat4f scale;
};


