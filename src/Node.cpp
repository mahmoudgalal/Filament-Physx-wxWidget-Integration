#include "Node.h"
#include <filament/TransformManager.h>
#include <math/mat4.h>
#include <math/vec3.h>

using namespace filament;
void Node::setPose(const float* mat4x4)
{
	auto& tcm = getEngine().getTransformManager();
	auto ci = tcm.getInstance(getRenderable());

	filament::math::mat4f t(mat4x4[0], mat4x4[1], mat4x4[2], mat4x4[3],
							mat4x4[4], mat4x4[5], mat4x4[6], mat4x4[7],
							mat4x4[8], mat4x4[9], mat4x4[10],mat4x4[11],
							mat4x4[12],mat4x4[13],mat4x4[14],mat4x4[15]
	);
	tcm.setTransform(ci, t * filament::math::mat4f::scaling(objscale));
}

void Node::setPosition(math::float3 const& position)
{
	auto ent = getRenderable();
	auto& tcm = getEngine().getTransformManager();
	auto ci = tcm.getInstance(ent);
	math::mat4f model = tcm.getTransform(ci);
	model[3].xyz = position;
	tcm.setTransform(ci, model);
}

void Node::rotateX(double angle)
{
	auto ent = getRenderable();
	auto& tcm = getEngine().getTransformManager();
	auto ci = tcm.getInstance(ent);
	math::mat4f model = tcm.getTransform(ci);
	math::float3 about{ 1,0,0 };
	auto rot = math::mat4f::rotation(angle, about);
	tcm.setTransform(ci, model * rot);
}

void Node::rotateY(double angle)
{
	auto ent = getRenderable();
	auto& tcm = getEngine().getTransformManager();
	auto ci = tcm.getInstance(ent);
	math::mat4f model = tcm.getTransform(ci);
	math::float3 about{ 0,1,0 };
	auto rot = math::mat4f::rotation(angle, about);
	tcm.setTransform(ci, model * rot);
}

void Node::rotateZ(double angle)
{
	auto ent = getRenderable();
	auto& tcm = getEngine().getTransformManager();
	auto ci = tcm.getInstance(ent);
	math::mat4f model = tcm.getTransform(ci);
	math::float3 about{ 0,0,1 };
	auto rot = math::mat4f::rotation(angle, about);
	tcm.setTransform(ci, model * rot);
}

void Node::rotateXYZ(float anglex, float angley, float anglez)
{
	auto ent = getRenderable();
	auto& tcm = getEngine().getTransformManager();
	auto ci = tcm.getInstance(ent);
	math::mat4f model = tcm.getTransform(ci);
	math::float3 about{ 0,1,0 };
	auto rot = math::mat4f::eulerZYX(anglez, angley, anglex);
	tcm.setTransform(ci, model * rot);
}

void Node::setScale(float sx, float sy, float sz)
{
	objscale = { sx,sy,sz };
	auto ent = getRenderable();
	auto& tcm = getEngine().getTransformManager();
	auto ci = tcm.getInstance(ent);
	math::mat4f model = tcm.getTransform(ci);
	model[0].x = sx;
	model[1].y = sy;
	model[2].z = sz;
	tcm.setTransform(ci, model);
}

void Node::setScale(float s)
{
	setScale(s, s, s);
}
