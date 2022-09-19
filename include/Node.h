#pragma once
#include <filament/Engine.h>
#include <utils/Entity.h>
#include <filament/RenderableManager.h>

class Node
{
public:
	void setPose(const float* pose);
	virtual filament::Engine& getEngine() = 0;
	virtual utils::Entity& getRenderable() = 0;
	virtual filament::math::float3 getScale() { return objscale; };
	virtual void setObjScale(filament::math::float3& s) { objscale = s; };
	virtual ~Node() {};
	virtual void setCastShadows(bool enabled) {
		auto& rcm = getEngine().getRenderableManager();
		auto instance = rcm.getInstance(getRenderable());
		rcm.setCastShadows(instance, enabled);
	}

private:
	filament::math::float3 objscale{ 1.0f,1.0f,1.0f };
};

