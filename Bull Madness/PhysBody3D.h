#ifndef __PhysBody3D_H__
#define __PhysBody3D_H__

#include "p2DynArray.h"
#include "p2List.h"
#include "glmath.h"

class btRigidBody;
class btCollisionShape;
struct btDefaultMotionState;
class Module;

class Sphere;
class Cube;
class Cylinder;
class Plane;

class Primitive;

class PhysBody3D
{
public:
	PhysBody3D();
	PhysBody3D(btRigidBody* body);
	~PhysBody3D();

	void SetBody(Sphere* primitive, float mass, bool sensor = false, bool enviroment_objects = false);
	void SetBody(Cube* primitive, vec3 size, float mass, bool sensor = false, bool enviroment_objects = false);
	void SetBody(Cylinder* primitive, float mass, bool sensor = false, bool enviroment_objects = false);
	bool HasBody() const;
	btRigidBody* GetBody() const;
	void SetAsSensor(bool sensor);

	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void ResetTransform();

	void SetPos(float x, float y, float z);
	void SetPos(vec3 position);
	vec3 GetPos() const;

	void SetSpeed(vec3 speed);
	void Push(vec3 force);
	void Stop();

	float DistanceFromWorldOrigin(vec3 bodyPos) const;
	float DistanceBetweenBodies(vec3 bodyPos) const;

private:
	void SetBody(btCollisionShape* shape, Primitive* parent, float mass, bool sensor = false, bool enviroment_objects = false);

	btRigidBody* body;
	btCollisionShape* colShape;
	btDefaultMotionState* motionState;

public:
	Primitive* parentPrimitive;
	p2List<Module*> collision_listeners;

	bool sensor;
	bool enviroment_objects;
};

#endif // __PhysBody3D_H__