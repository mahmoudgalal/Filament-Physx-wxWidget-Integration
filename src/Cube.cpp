#include "Cube.h"
#include <utils/EntityManager.h>
#include <filament/VertexBuffer.h>
#include <filament/IndexBuffer.h>
#include <filament/RenderableManager.h>
#include <filament/TransformManager.h>

using namespace filament::math;
using namespace filament;

const uint32_t Cube::mIndices[] = {
	// solid
		2,0,1, 2,1,3,  // far
		5,4,6, 7,5,6,  // near
		4,0,2, 6,4,2,  // left
		3,1,5, 3,5,7,  // right
		0,4,5, 0,5,1,  // bottom
		7,6,2, 3,7,2,  // top

	// wire-frame
	0,1, 1,3, 3,2, 2,0,     // far
	4,5, 5,7, 7,6, 6,4,     // near
	0,4, 1,5, 3,7, 2,6,
};

const filament::math::float3 Cube::mVertices[] = {
		{ -1, -1,  1},  // 0. left bottom far
		{  1, -1,  1},  // 1. right bottom far
		{ -1,  1,  1},  // 2. left top far
		{  1,  1,  1},  // 3. right top far
		{ -1, -1, -1},  // 4. left bottom near
		{  1, -1, -1},  // 5. right bottom near
		{ -1,  1, -1},  // 6. left top near
		{  1,  1, -1}  // 7. right top   
};

Cube::Cube(Engine& engine, filament::MaterialInstance* materialInstance,
	float3 linearColor,
	bool culling, RenderMode mode) :
	mEngine(engine),
	mMaterialInstance(materialInstance),
	renderMode(mode) {

	mVertexBuffer = VertexBuffer::Builder()
		.vertexCount(8)
		.bufferCount(1)
		.attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT3)
		.build(engine);

	mIndexBuffer = IndexBuffer::Builder()
		.indexCount(12 * 2 + 3 * 2 * 6)
		.build(engine);

	mVertexBuffer->setBufferAt(engine, 0,
		VertexBuffer::BufferDescriptor(
			mVertices, mVertexBuffer->getVertexCount() * sizeof(mVertices[0])));

	mIndexBuffer->setBuffer(engine,
		IndexBuffer::BufferDescriptor(
			mIndices, mIndexBuffer->getIndexCount() * sizeof(uint32_t)));

	utils::EntityManager& em = utils::EntityManager::get();
	mSolidRenderable = em.create();
	RenderableManager::Builder(1)
		.boundingBox({ { 0, 0, 0 },
					  { 1, 1, 1 } })
		.material(0, mMaterialInstance)
		.geometry(0, RenderableManager::PrimitiveType::TRIANGLES, mVertexBuffer, mIndexBuffer, 0, 3 * 2 * 6)
		.priority(7)
		.castShadows(false)
		.receiveShadows(true)
		.culling(culling)
		.build(engine, mSolidRenderable);

	mWireFrameRenderable = em.create();
	RenderableManager::Builder(1)
		.boundingBox({ { 0, 0, 0 },
					  { 1, 1, 1 } })
		.material(0, mMaterialInstance)
		.geometry(0, RenderableManager::PrimitiveType::LINES, mVertexBuffer, mIndexBuffer, WIREFRAME_OFFSET, 24)
		.priority(6)		
		.culling(culling)
		.build(engine, mWireFrameRenderable);
}


void Cube::rotateX(double angle)
{
	auto& tcm = this->mEngine.getTransformManager();
	rotation = filament::math::mat4f::rotation(angle,
		filament::math::float3{ 1, 0, 0 });
	tcm.setTransform(tcm.getInstance(this->getSolidRenderable()),
		translation * rotation * scale);
}

void Cube::rotateY(double angle)
{
	auto& tcm = this->mEngine.getTransformManager();
	rotation = filament::math::mat4f::rotation(angle,
		filament::math::float3{ 0, 1, 0 });
	tcm.setTransform(tcm.getInstance(this->getSolidRenderable()),
		translation * rotation * scale);
}

void Cube::rotateZ(double angle)
{
	auto& tcm = this->mEngine.getTransformManager();
	rotation = filament::math::mat4f::rotation(angle,
		filament::math::float3{ 0, 0, 1 });
	tcm.setTransform(tcm.getInstance(this->getSolidRenderable()),
		translation * rotation * scale);
}

void Cube::rotateXYZ(double angle) {
	auto& tcm = this->mEngine.getTransformManager();
	rotation = filament::math::mat4f::eulerZYX(angle, angle, angle);
	//filament::math::float3{ 0, 0, 1 });
	tcm.setTransform(tcm.getInstance(this->getSolidRenderable()),
		translation * rotation * scale);
}


void Cube::setPosition(filament::math::float3 const& position) {
	auto& tcm = mEngine.getTransformManager();
	auto ci = tcm.getInstance(getRenderable());
	//mat4f model = tcm.getTransform(ci);
   // model[3].xyz = position;
	translation = mat4f::translation(position);
	
	tcm.setTransform(ci, translation * rotation * scale);
}

void Cube::setScale(float s)
{
	setScale(s, s, s);
}

void Cube::setScale(float sx, float sy, float sz)
{
	auto& tcm = this->mEngine.getTransformManager();
	auto ci = tcm.getInstance(getRenderable());
	math::float3 scalVec  = {sx, sy, sz };
	scale = mat4f::scaling(scalVec);
	tcm.setTransform(ci, translation * rotation * scale);
}

Cube::~Cube() {
	mEngine.destroy(mVertexBuffer);
	mEngine.destroy(mIndexBuffer);
	mEngine.destroy(mMaterialInstance);
	// We don't own the material, only instances
	mEngine.destroy(mSolidRenderable);
	mEngine.destroy(mWireFrameRenderable);

	utils::EntityManager& em = utils::EntityManager::get();
	em.destroy(mSolidRenderable);
	em.destroy(mWireFrameRenderable);
}

