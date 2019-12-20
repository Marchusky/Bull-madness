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

	//Bull Body------------------------------------------------
	chassis = Cube(vec3(info.chassis_size.x +0.5f, info.chassis_size.y, info.chassis_size.z + 0.3f));
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis.transform);
	btQuaternion q = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	chassis.transform.M[12] += offset.getX();
	chassis.transform.M[13] += offset.getY();
	chassis.transform.M[14] += offset.getZ();

	chassis.Render();
	

	//Bull Head--------------------------------------------------------
	Cube front(vec3(1, info.chassis_size.y - 0.5f, 2));
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&front.transform);
	btVector3 front_offset(info.chassis_offset.x, info.chassis_offset.y + 0.3, info.chassis_offset.z +1.6);
	front_offset = front_offset.rotate(q.getAxis(), q.getAngle());
	
	front.transform.M[12] += front_offset.getX();
	front.transform.M[13] += front_offset.getY();
	front.transform.M[14] += front_offset.getZ();

	front.Render();

	//Bull left horn---------------------------------------------------------
	Cube horn(vec3(1, 0.4f, 0.3f));
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&horn.transform);
	btVector3 horn_offset(info.chassis_offset.x + 0.8, info.chassis_offset.y + 0.7, info.chassis_offset.z + 1.8);
	horn_offset = horn_offset.rotate(q.getAxis(), q.getAngle());

	horn.transform.M[12] += horn_offset.getX();
	horn.transform.M[13] += horn_offset.getY();
	horn.transform.M[14] += horn_offset.getZ();

	horn.Render();

	//Bull left horn extension
	Cube horn_Extension(vec3(0.3f, 0.4f, 1.0f));
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&horn_Extension.transform);
	btVector3 horn_Extension_offset(info.chassis_offset.x + 1.15f, info.chassis_offset.y + 0.7f, info.chassis_offset.z + 2.3f);
	horn_Extension_offset = horn_Extension_offset.rotate(q.getAxis(), q.getAngle());

	horn_Extension.transform.M[12] += horn_Extension_offset.getX();
	horn_Extension.transform.M[13] += horn_Extension_offset.getY();
	horn_Extension.transform.M[14] += horn_Extension_offset.getZ();

	horn_Extension.Render();

	//Bull right horn------------------------------------------------------
	Cube horn1(vec3(1, 0.4, 0.3));
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&horn1.transform);
	btVector3 horn1_offset(info.chassis_offset.x - 0.8, info.chassis_offset.y + 0.7, info.chassis_offset.z + 1.8);
	horn1_offset = horn1_offset.rotate(q.getAxis(), q.getAngle());

	horn1.transform.M[12] += horn1_offset.getX();
	horn1.transform.M[13] += horn1_offset.getY();
	horn1.transform.M[14] += horn1_offset.getZ();

	horn1.Render();

	//Bull right horn extension
	Cube horn1_Extension(vec3(0.3f, 0.4f, 1.0f));
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&horn1_Extension.transform);
	btVector3 horn1_Extension_offset(info.chassis_offset.x - 1.15f, info.chassis_offset.y + 0.7f, info.chassis_offset.z + 2.3f);
	horn1_Extension_offset = horn1_Extension_offset.rotate(q.getAxis(), q.getAngle());

	horn1_Extension.transform.M[12] += horn1_Extension_offset.getX();
	horn1_Extension.transform.M[13] += horn1_Extension_offset.getY();
	horn1_Extension.transform.M[14] += horn1_Extension_offset.getZ();

	horn1_Extension.Render();

	//Bull tail
	Cube tail = Cube(vec3(0.25f, 0.25, 0.35));
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&tail.transform);
	btVector3 tail_offset(info.chassis_offset.x, info.chassis_offset.y+0.5f, info.chassis_offset.z-1.8);
	tail_offset = tail_offset.rotate(q.getAxis(), q.getAngle());

	tail.transform.M[12] += tail_offset.getX();
	tail.transform.M[13] += tail_offset.getY();
	tail.transform.M[14] += tail_offset.getZ();

	tail.Render();

	//Bull tail extension
	Cube tail_Extension = Cube(vec3(0.2f, 0.8f, 0.3f));
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&tail_Extension.transform);
	btVector3 tail_Extension_offset(info.chassis_offset.x, info.chassis_offset.y+0.15f, info.chassis_offset.z - 2.f);
	tail_Extension_offset = tail_Extension_offset.rotate(q.getAxis(), q.getAngle());

	tail_Extension.transform.M[12] += tail_Extension_offset.getX();
	tail_Extension.transform.M[13] += tail_Extension_offset.getY();
	tail_Extension.transform.M[14] += tail_Extension_offset.getZ();

	tail_Extension.Render();


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