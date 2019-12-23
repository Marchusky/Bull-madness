#include "PhysBody3D.h"
#include "Globals.h"
#include "Application.h"
#include "Primitive.h"
#include "glmath.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// ---------------------------------------------------------
PhysBody3D::PhysBody3D()
	: body(nullptr)
	, colShape(nullptr)
	, motionState(nullptr)
	, parentPrimitive(nullptr)
	, sensor(false)
{

}

PhysBody3D::PhysBody3D(btRigidBody* body)
	: body(body)
	, colShape(nullptr)
	, motionState(nullptr)
	, parentPrimitive(nullptr)
{}

// ---------------------------------------------------------
PhysBody3D::~PhysBody3D()
{
	if (HasBody() == true)
	{
		App->physics-> RemoveBodyFromWorld(body);
		delete body;
		delete colShape;
		delete motionState;
	}
}

void PhysBody3D::SetBody(Sphere* primitive, float mass, bool sensor, bool enviroment_objects)
{
	SetBody(new btSphereShape(primitive->GetRadius()),
		primitive, mass, sensor, enviroment_objects);
}

void PhysBody3D::SetBody(Cube* primitive, vec3 size, float mass, bool sensor, bool enviroment_objects)
{
	btVector3 btSize = { size.x, size.y, size.z };

	SetBody(new btBoxShape(btSize * HALF),
		primitive, mass, sensor, enviroment_objects);
}

void PhysBody3D::SetBody(Cylinder* primitive, float mass, bool sensor, bool enviroment_objects)
{
	btVector3 btSize = { primitive->GetHeight() * HALF, primitive->GetRadius(), 0.0f };
	
	SetBody(new btCylinderShapeX(btSize),
		primitive, mass, sensor, enviroment_objects);
}

bool PhysBody3D::HasBody() const
{
	return body != nullptr;
}

btRigidBody * PhysBody3D::GetBody() const
{
	return body;
}

// ---------------------------------------------------------
void PhysBody3D::GetTransform(float* matrix) const
{
	if (HasBody() == false)
		return;

	body->getWorldTransform().getOpenGLMatrix(matrix);
}

// ---------------------------------------------------------
void PhysBody3D::SetTransform(const float* matrix) const
{
	if (HasBody() == false)
		return;

	btTransform trans;
	trans.setFromOpenGLMatrix(matrix);
	body->setWorldTransform(trans);
	body->activate();
}

// ---------------------------------------------------------
void PhysBody3D::SetPos(float x, float y, float z)
{
	if (HasBody() == false)
		return;

	btTransform trans = body->getWorldTransform();
	trans.setOrigin(btVector3(x, y, z));
	body->setWorldTransform(trans);
	body->activate();
}

void PhysBody3D::SetPos(vec3 position)
{
	if (HasBody() == false)
		return;

	btVector3 origin = { position.x, position.y, position.z };

	btTransform trans = body->getWorldTransform();
	trans.setOrigin(origin);
	body->setWorldTransform(trans);
	body->activate();
}

vec3 PhysBody3D::GetPos() const
{
	if (HasBody() == false)
		return vec3(0, 0, 0);

	btTransform trans = body->getWorldTransform();
	btVector3 buffer = trans.getOrigin();

	vec3 position = {buffer.getX(), buffer.getY(), buffer.getZ()};

	return position;
}

float PhysBody3D::DistanceFromWorldOrigin(vec3 bodyPos) const
{
	vec3 origin(0.0f, 0.0f, 0.0f);

	float posX = origin.x - bodyPos.x;
	float posY = origin.y - bodyPos.y;
	float posZ = origin.z - bodyPos.z;

	if (posX < 0)
	{
		posX = posX * (-1);
	}

	if (posY < 0)
	{
		posY = posY * (-1);
	}

	if (posZ < 0)
	{
		posZ = posZ * (-1);
	}

	float distance = posX + posY + posZ;

	return distance;
}

float PhysBody3D::DistanceBetweenBodies(vec3 bodyPos) const
{
	vec3 firstBody = this->GetPos();

	float posX = firstBody.x - bodyPos.x;
	float posY = firstBody.y - bodyPos.y;
	float posZ = firstBody.z - bodyPos.z;

	if (posX < 0)
	{
		posX = posX * (-1);
	}

	if (posY < 0)
	{
		posY = posY * (-1);
	}

	if (posZ < 0)
	{
		posZ = posZ * (-1);
	}

	float distance = posX + posY + posZ;

	return distance;
}

void PhysBody3D::SetAsSensor(bool sensor)
{
	if (this->sensor != sensor)
	{
		this->sensor = sensor;
		if (sensor == true)
		{
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);		//Add “CF_NO_CONTACT_RESPONSE”” to Current Flags
		}
		else
		{
			body->setCollisionFlags(body->getCollisionFlags() &~ btCollisionObject::CF_NO_CONTACT_RESPONSE);	//Remove “CF_NO_CONTACT_RESPONSE” from Current Flags
		}
	}
}

// --- Resets the transform of a body to a neutral state (IdentityMatrix)
void PhysBody3D::ResetTransform()
{
	mat4x4 matrix = IdentityMatrix;
	btTransform transform;
	transform.setFromOpenGLMatrix(&matrix);

	body->setWorldTransform(transform);
}

void PhysBody3D::SetSpeed(vec3 speed)
{
	Stop();
	Push(speed);
}

void PhysBody3D::Push(vec3 force)
{
	if (HasBody())
	{
		body->activate();
		body->applyCentralForce(btVector3(force.x, force.y, force.z));
	}
}

void PhysBody3D::Stop()
{
	if (HasBody())
		body->clearForces();
}

void PhysBody3D::SetBody(btCollisionShape * shape, Primitive* parent, float mass, bool sensor, bool enviroment_objects)
{
	assert(HasBody() == false);

	parentPrimitive = parent;

	colShape = shape;

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&parent->transform);

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	motionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);

	body = new btRigidBody(rbInfo);

	body->setUserPointer(this);

	this->SetAsSensor(sensor);

	if (sensor == true)								//With this both players will detect a collision with the sensor.
	{
		this->collision_listeners.add(App->player);
	}

	this->enviroment_objects = enviroment_objects;				//Way to keep track of elements in the physics world that are part of the arena.

 	App->physics->AddBodyToWorld(body);
}
