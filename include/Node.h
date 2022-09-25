#pragma once
#include <filament/Engine.h>
#include <utils/Entity.h>
#include <filament/RenderableManager.h>
#include<string>

class Node
{
public:
	struct RGBColor {
		float r;
		float g;
		float b;
		float alpha;
	};
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
	void setName(std::string& name) {
		this->name = name;
	}
	const std::string& getName() const{
		return name;
	}
	//void setSleepColor(RGBColor& color) { this->sleepColor = color; }
	//const RGBColor& getSleepColor() const { return sleepColor; }
	virtual void onSleep(bool isSleeping) {};

private:
	filament::math::float3 objscale{ 1.0f,1.0f,1.0f };
	std::string name{"Base_Node"};
	//RGBColor sleepColor;

};

