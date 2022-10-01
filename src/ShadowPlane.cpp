#include "ShadowPlane.h"
#include <math/norm.h>
#include <filament/RenderableManager.h>
#include <utils/EntityManager.h>
#include <filament/TransformManager.h>

using namespace filament;
using namespace math;


ShadowPlane::ShadowPlane(Engine& engine, MaterialInstance* materialInstance): mEngine(engine),
mMaterialInstance(materialInstance)
{
    const static uint32_t indices[]{
        0, 1, 2, 2, 3, 0
    };
    const static float3 vertices[]{
        { -10, 0, -10 },
        { -10, 0,  10 },
        {  10, 0,  10 },
        {  10, 0, -10 },
    };

    short4 tbn = packSnorm16(normalize(positive(mat3f{
        float3{1.0f, 0.0f, 0.0f}, float3{0.0f, 0.0f, 1.0f}, float3{0.0f, 1.0f, 0.0f}
        }.toQuaternion())).xyzw);
    const static short4 normals[]{ tbn, tbn, tbn, tbn };
    mVertexBuffer = VertexBuffer::Builder()
        .vertexCount(4)
        .bufferCount(2)
        .attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT3)
        .attribute(VertexAttribute::TANGENTS, 1, VertexBuffer::AttributeType::SHORT4)
        .normalized(VertexAttribute::TANGENTS)
        .build(engine);
    mVertexBuffer->setBufferAt(engine, 0, VertexBuffer::BufferDescriptor(
        vertices, mVertexBuffer->getVertexCount() * sizeof(vertices[0])));
    mVertexBuffer->setBufferAt(engine, 1, VertexBuffer::BufferDescriptor(
        normals, mVertexBuffer->getVertexCount() * sizeof(normals[0])));
    mIndexBuffer = IndexBuffer::Builder().indexCount(6).build(engine);
    mIndexBuffer->setBuffer(engine, IndexBuffer::BufferDescriptor(
        indices, mIndexBuffer->getIndexCount() * sizeof(uint32_t)));

    auto& em = utils::EntityManager::get();
    mRenderable = em.create();
    RenderableManager::Builder(1)
        .boundingBox({ { 0, 0, 0 }, { 10, 1e-4f, 10 } })
        .material(0, mMaterialInstance)
        .geometry(0, RenderableManager::PrimitiveType::TRIANGLES, mVertexBuffer, mIndexBuffer, 0, 6)
        .culling(false)
        .receiveShadows(true)
        .castShadows(false)
        .build(engine, mRenderable);

    //auto& tcm = engine.getTransformManager();
    //tcm.setTransform(tcm.getInstance(renderable), mat4f::translation(float3{ 0, -1, -4 }));

}

void ShadowPlane::rotateX(double angle)
{
	auto& tcm = this->mEngine.getTransformManager();
	rotation = filament::math::mat4f::rotation(angle,
		filament::math::float3{ 1, 0, 0 });
	tcm.setTransform(tcm.getInstance(this->getRenderable()),
		translation * rotation * scale);
}

void ShadowPlane::rotateY(double angle)
{
	auto& tcm = this->mEngine.getTransformManager();
	rotation = filament::math::mat4f::rotation(angle,
		filament::math::float3{ 0, 1, 0 });
	tcm.setTransform(tcm.getInstance(this->getRenderable()),
		translation * rotation * scale);
}

void ShadowPlane::rotateZ(double angle)
{
	auto& tcm = this->mEngine.getTransformManager();
	rotation = filament::math::mat4f::rotation(angle,
		filament::math::float3{ 0, 0, 1 });
	tcm.setTransform(tcm.getInstance(this->getRenderable()),
		translation * rotation * scale);
}

void ShadowPlane::rotateXYZ(double angle) {
	auto& tcm = this->mEngine.getTransformManager();
	rotation = filament::math::mat4f::eulerZYX(angle, angle, angle);
	//filament::math::float3{ 0, 0, 1 });
	tcm.setTransform(tcm.getInstance(this->getRenderable()),
		translation * rotation * scale);
}


void ShadowPlane::setPosition(filament::math::float3 const& position) {
	auto& tcm = mEngine.getTransformManager();
	auto ci = tcm.getInstance(getRenderable());
	//mat4f model = tcm.getTransform(ci);
   // model[3].xyz = position;
	translation = mat4f::translation(position);

	tcm.setTransform(ci, translation * rotation * scale);
}

ShadowPlane::~ShadowPlane() {
	mEngine.destroy(mVertexBuffer);
	mEngine.destroy(mIndexBuffer);
	mEngine.destroy(mMaterialInstance);
	// We don't own the material, only instances
	mEngine.destroy(mRenderable);

	utils::EntityManager& em = utils::EntityManager::get();
	em.destroy(mRenderable);
}
