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
	virtual void setPosition(filament::math::float3 const& position);
	virtual void rotateX(double angle);
	virtual void rotateY(double angle);
	virtual void rotateZ(double angle);
	virtual void rotateXYZ(float anglex, float angley, float anglez);
	virtual void setScale(float sx, float sy, float sz);
	virtual void setScale(float s);

	virtual ~Node() {};
	virtual void setCastShadows(bool enabled) {
		auto& rcm = getEngine().getRenderableManager();
		auto instance = rcm.getInstance(getRenderable());		
		rcm.setCastShadows(instance, enabled);
	}
	void setName(std::string& name) {
		this->name = name;
	}
	const std::string& getName() const {
		return name;
	}
	//void setSleepColor(RGBColor& color) { this->sleepColor = color; }
	//const RGBColor& getSleepColor() const { return sleepColor; }
	virtual void onSleep(bool isSleeping) {};

private:
	filament::math::float3 objscale{ 1.0f,1.0f,1.0f };
	std::string name{ "Base_Node" };

};

