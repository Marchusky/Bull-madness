#include "PhysVehicle3D.h"
#include "Primitive.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// ----------------------------------------------------------------------------
VehicleInfo::~VehicleInfo()
{
	//if(wheels != NULL)
		//delete wheels;
}

// ----------------------------------------------------------------------------
PhysVehicle3D::PhysVehicle3D(btRigidBody* body, btRaycastVehicle* vehicle, const VehicleInfo& info) : PhysBody3D(body), vehicle(vehicle), info(info)
{
}

// ----------------------------------------------------------------------------
PhysVehicle3D::~PhysVehicle3D()
{
	delete vehicle;
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Render()
{
	Cylinder* wheel;

	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		wheel = new Cylinder(info.wheels[i].radius, info.wheels[i].width);
		wheel->color = Blue;

		vehicle->updateWheelTransform(i);
		vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(&wheel->transform);

		wheel->Render();
		delete wheel;
		wheel = nullptr;
	}

	chassis = Cube(vec3(info.chassis_size.x, info.chassis_size.y, info.chassis_size.z));
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis.transform);
	btQuaternion q = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	chassis.transform.M[12] += offset.getX();
	chassis.transform.M[13] += offset.getY();
	chassis.transform.M[14] += offset.getZ();

	chassis.Render();
	
	Cube front(vec3(1, info.chassis_size.y, 2));
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&front.transform);
	btVector3 front_offset(info.chassis_offset.x, info.chassis_offset.y + 0.3, info.chassis_offset.z +1.6);
	front_offset = front_offset.rotate(q.getAxis(), q.getAngle());
	
	front.transform.M[12] += front_offset.getX();
	front.transform.M[13] += front_offset.getY();
	front.transform.M[14] += front_offset.getZ();

	front.Render();

	Cube horn(vec3(1, 0.4, 0.3));
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&horn.transform);
	btVector3 horn_offset(info.chassis_offset.x + 0.8, info.chassis_offset.y + 0.7, info.chassis_offset.z + 1.8);
	horn_offset = horn_offset.rotate(q.getAxis(), q.getAngle());

	horn.transform.M[12] += horn_offset.getX();
	horn.transform.M[13] += horn_offset.getY();
	horn.transform.M[14] += horn_offset.getZ();

	horn.Render();

	Cube horn1(vec3(1, 0.4, 0.3));
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&horn1.transform);
	btVector3 horn1_offset(info.chassis_offset.x - 0.8, info.chassis_offset.y + 0.7, info.chassis_offset.z + 1.8);
	horn1_offset = horn1_offset.rotate(q.getAxis(), q.getAngle());

	horn1.transform.M[12] += horn1_offset.getX();
	horn1.transform.M[13] += horn1_offset.getY();
	horn1.transform.M[14] += horn1_offset.getZ();

	horn1.Render();
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::ApplyEngineForce(float force)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Brake(float force)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].brake == true)
		{
			vehicle->setBrake(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Turn(float degrees)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].steering == true)
		{
			vehicle->setSteeringValue(degrees, i);
		}
	}
}

// ----------------------------------------------------------------------------
float PhysVehicle3D::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}