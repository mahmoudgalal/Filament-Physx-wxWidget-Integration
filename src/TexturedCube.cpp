#include "TexturedCube.h"

#include <utils/EntityManager.h>
#include <filament/VertexBuffer.h>
#include <filament/IndexBuffer.h>
#include <filament/RenderableManager.h>
#include <filament/TransformManager.h>

using namespace filament::math;
using namespace filament;

const uint32_t TexturedCube::mIndices[] = {
	// solid
		2,0,1, 2,1,3,  // far
		5,4,6, 7,5,6,  // near
		8,9,10, 11,8,10,  // left
		14,13,12, 14,12,15,  // right
		16,17,18, 18,19,16,  // top
		22,21,20, 20,23,22  // bottom

};

const filament::math::float3 TexturedCube::mVertices[] = {
	{-1.0f,  1.0f, -1.0f}, /* Back. */
	{1.0f,  1.0f, -1.0f },
	{-1.0f, -1.0f, -1.0f},
	{1.0f, -1.0f, -1.0f },
	{-1.0f,  1.0f,  1.0f}, /* Front. */
	{1.0f,  1.0f,  1.0f},
	{-1.0f, -1.0f,  1.0f},
	{1.0f, -1.0f,  1.0f},
	{-1.0f,  1.0f, -1.0f}, /* Left. */
	{-1.0f, -1.0f, -1.0f},
	{-1.0f, -1.0f,  1.0f},
	{-1.0f,  1.0f,  1.0f},
	{1.0f,  1.0f, -1.0f}, /* Right. */
	{1.0f, -1.0f, -1.0f},
	{1.0f, -1.0f,  1.0f},
	{1.0f,  1.0f,  1.0f},
	{-1.0f, 1.0f, -1.0f}, /* Top. */
	{-1.0f, 1.0f,  1.0f},
	{1.0f, 1.0f,  1.0f},
	{1.0f, 1.0f, -1.0f},
	{-1.0f, -1.0f, -1.0f}, /* Bottom. */
	{-1.0f,  -1.0f, 1.0f},
	{1.0f, -1.0f,  1.0f},
	{1.0f,  -1.0f, -1.0f},

	// Tex Coordintes
	{1.0f, 1.0f,0.0f}, /* Back. */
	{0.0f, 1.0f,0.0f},
	{1.0f, 0.0f,0.0f},
	{0.0f, 0.0f,0.0f},
	{0.0f, 1.0f,0.0f}, /* Front. */
	{1.0f, 1.0f,0.0f},
	{0.0f, 0.0f,0.0f},
	{1.0f, 0.0f,0.0f},
	{0.0f, 1.0f,0.0f}, /* Left. */
	{0.0f, 0.0f,0.0f},
	{1.0f, 0.0f,0.0f},
	{1.0f, 1.0f,0.0f},
	{1.0f, 1.0f,0.0f}, /* Right. */
	{1.0f, 0.0f,0.0f},
	{0.0f, 0.0f,0.0f},
	{0.0f, 1.0f,0.0f},
	{0.0f, 1.0f,0.0f}, /* Top. */
	{0.0f, 0.0f,0.0f},
	{1.0f, 0.0f,0.0f},
	{1.0f, 1.0f,0.0f},
	{0.0f, 0.0f,0.0f}, /* Bottom. */
	{0.0f, 1.0f,0.0f},
	{1.0f, 1.0f,0.0f},
	{1.0f, 0.0f,0.0f}
};

TexturedCube::TexturedCube(Engine& engine,
	filament::MaterialInstance* material,
	float3 linearColor,
	bool culling) :
	mEngine(engine),
	mMaterialInstanceSolid(material) {

	mVertexBuffer = VertexBuffer::Builder()
		.vertexCount(24)
		.bufferCount(2)
		.attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT3)
		.attribute(VertexAttribute::UV0, 1, VertexBuffer::AttributeType::FLOAT3, 0, 0)
		.build(engine);

	mIndexBuffer = IndexBuffer::Builder()
		.indexCount(3 * 2 * 6)
		.build(engine);

	mVertexBuffer->setBufferAt(engine, 0,
		VertexBuffer::BufferDescriptor(
			mVertices, mVertexBuffer->getVertexCount() * sizeof(mVertices[0])));
	mVertexBuffer->setBufferAt(engine, 1,
		VertexBuffer::BufferDescriptor(
			&mVertices[24], mVertexBuffer->getVertexCount() * sizeof(mVertices[0])));

	mIndexBuffer->setBuffer(engine,
		IndexBuffer::BufferDescriptor(
			mIndices, mIndexBuffer->getIndexCount() * sizeof(uint32_t)));

	utils::EntityManager& em = utils::EntityManager::get();
	mSolidRenderable = em.create();
	RenderableManager::Builder(1)
		.boundingBox({ { 0, 0, 0 },
					  { 1, 1, 1 } })
		.material(0, mMaterialInstanceSolid)
		.geometry(0, RenderableManager::PrimitiveType::TRIANGLES, mVertexBuffer, mIndexBuffer, 0, 3 * 2 * 6)
		.priority(7)
		.castShadows(true)
		.receiveShadows(false)
		.culling(culling)
		.build(engine, mSolidRenderable);
}

void TexturedCube::rotateX(double angle)
{
	auto& tcm = this->mEngine.getTransformManager();
	rotation = filament::math::mat4f::rotation(angle,
		filament::math::float3{ 1, 0, 0 });
	tcm.setTransform(tcm.getInstance(this->getRenderable()),
		translation * rotation * scale);
}

void TexturedCube::rotateY(double angle)
{
	auto& tcm = this->mEngine.getTransformManager();
	rotation = filament::math::mat4f::rotation(angle,
		filament::math::float3{ 0, 1, 0 });
	tcm.setTransform(tcm.getInstance(this->getRenderable()),
		translation * rotation * scale);
}

void TexturedCube::rotateZ(double angle)
{
	auto& tcm = this->mEngine.getTransformManager();
	rotation = filament::math::mat4f::rotation(angle,
		filament::math::float3{ 0, 0, 1 });
	tcm.setTransform(tcm.getInstance(this->getRenderable()),
		translation * rotation * scale);
}

void TexturedCube::rotateXYZ(double angle) {
	auto& tcm = this->mEngine.getTransformManager();
	rotation = filament::math::mat4f::eulerZYX(angle, angle, angle);
	//filament::math::float3{ 0, 0, 1 });
	tcm.setTransform(tcm.getInstance(this->getRenderable()),
		translation * rotation * scale);
}

void TexturedCube::setPosition(filament::math::float3 const& position) {
	auto& tcm = mEngine.getTransformManager();
	auto ci = tcm.getInstance(getRenderable());
	//mat4f model = tcm.getTransform(ci);
   // model[3].xyz = position;
	translation = mat4f::translation(position);
	tcm.setTransform(ci, translation * rotation * scale);
}

void TexturedCube::setScale(float s)
{
	setScale(s, s, s);
}

void TexturedCube::setScale(float sx, float sy, float sz)
{
	auto& tcm = this->mEngine.getTransformManager();
	auto ci = tcm.getInstance(getRenderable());
	math::float3 scalVec = { sx, sy, sz };
	scale = mat4f::scaling(scalVec);

	mat4f model = tcm.getTransform(ci);
	model[0].x = sx;
	model[1].y = sy;
	model[2].z = sz;

	tcm.setTransform(ci, model);
}

TexturedCube::~TexturedCube() {
	mEngine.destroy(mVertexBuffer);
	mEngine.destroy(mIndexBuffer);
	mEngine.destroy(mMaterialInstanceSolid);
	mMaterialInstanceSolid = nullptr;
	// We don't own the material, only instances
	mEngine.destroy(mSolidRenderable);

	utils::EntityManager& em = utils::EntityManager::get();
	em.destroy(mSolidRenderable);
}
