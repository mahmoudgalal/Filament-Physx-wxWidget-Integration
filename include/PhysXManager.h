#pragma once

#include "PxPhysicsAPI.h"
#include "Node.h"
#include<vector>
using namespace physx;

class PhysXManager
{
public:
	PhysXManager();
	~PhysXManager();
	void simulate(float delta);
	void createCube(float x, float y, float z,const Node* node , const PxVec3& velocity = PxVec3(0));
	void createStaticCube(const PxVec3& pos, const PxVec3& scale, const Node* node, const PxVec3& velocity = PxVec3(0));

	void createSphere(float x, float y, float z, float radius, const Node* node , const PxVec3& velocity = PxVec3(0));

	void updateNodes();

private:
	void initializePhysX();

	PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;

	PxFoundation* gFoundation = NULL;
	PxPhysics* gPhysics = NULL;

	PxDefaultCpuDispatcher* gDispatcher = NULL;
	PxScene* gScene = NULL;

	PxMaterial* gMaterial = NULL;

	PxPvd* gPvd = NULL;
	const char* PVD_HOST = "127.0.0.1";
};

