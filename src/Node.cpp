#include "Node.h"
#include <filament/TransformManager.h>
#include <math/mat4.h>
#include <math/vec3.h>

void Node::setPose(const float* mat4x4)
{
	auto& tcm = getEngine().getTransformManager();
	auto ci = tcm.getInstance(getRenderable());

	filament::math::mat4f t(mat4x4[0] , mat4x4[1], mat4x4[2], mat4x4[3],
							mat4x4[4], mat4x4[5], mat4x4[6], mat4x4[7],
							mat4x4[8], mat4x4[9], mat4x4[10], mat4x4[11],
							mat4x4[12], mat4x4[13], mat4x4[14], mat4x4[15]
	);
	tcm.setTransform(ci, t  *filament::math::mat4f::scaling( getScale() ));
}
