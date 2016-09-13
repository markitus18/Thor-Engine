#include "PhysVehicle3D.h"
#include "Primitive.h"

// ----------------------------------------------------------------------------
VehicleInfo::~VehicleInfo()
{
}

// ----------------------------------------------------------------------------
PhysVehicle3D::PhysVehicle3D(btRigidBody* body, btRaycastVehicle* vehicle, const VehicleInfo& info) : PhysBody3D(body), vehicle(vehicle), info(info)
{
}

// ----------------------------------------------------------------------------
PhysVehicle3D::~PhysVehicle3D()
{
	if (info.wheels != NULL)
		delete[] info.wheels;


	delete vehicle;
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Render()
{
	//Wheels render
	Cylinder wheel;
	Cube wheelRadius;
	Cube joints;

	wheelRadius.color = Green;
	wheel.color = Blue;

	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{	
		wheel.radius = info.wheels[i].radius;
		wheel.height = info.wheels[i].width;

		wheelRadius.size = { info.wheels[i].width+0.1f, info.wheels[i].radius * 2, 0.1f };

		//vehicle->updateWheelTransform(i);
		vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(&wheel.transform);
		wheelRadius.transform = wheel.transform;

		wheelRadius.Render();
		wheel.Render();

	}

	//Chassis render
	Cube chassis(info.chassis_size.x, info.chassis_size.y, info.chassis_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis.transform);
	btQuaternion q = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	chassis.transform.M[12] += offset.getX();
	chassis.transform.M[13] += offset.getY();
	chassis.transform.M[14] += offset.getZ();

	chassis.Render();

	//turret render
	Sphere turret;
	turret.radius = info.turret.turretRadius;
	float matrix[16];
	info.turret.turret->GetTransform(matrix);
	mat4x4 transform(matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5], matrix[6], matrix[7], matrix[8], matrix[9], matrix[10], matrix[11], matrix[12], matrix[13], matrix[14], matrix[15]);
	turret.transform = transform;

	turret.Render();

	//Turret base render
	Cylinder turretBase;
	turretBase.color = Blue;
	float x, y, z;
	info.turret.turret->GetPos(&x,&y,&z);
	
	turretBase.transform.translate(x, y - 1, z);
	turretBase.SetRotation(90, {0,0,1});
	turretBase.height = 2.0f;
	turretBase.radius = turret.radius + 0.3;
	turretBase.Render();

	//Canon render
	Cylinder canon;
	canon.height = info.turret.canonLength;
	canon.radius = info.turret.canonRadius;
	info.turret.canon->GetTransform(matrix);
	mat4x4 transform2(matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5], matrix[6], matrix[7], matrix[8], matrix[9], matrix[10], matrix[11], matrix[12], matrix[13], matrix[14], matrix[15]);
	canon.transform = transform2;
	canon.Render();
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::ApplyLeftEngineForce(float force)
{
	for(int i = 0; i < vehicle->getNumWheels() / 2; ++i)
	{
		if(info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce(force, i);
		}
	}
}

void PhysVehicle3D::ApplyRightEngineForce(float force)
{
	for (int i = vehicle->getNumWheels() / 2; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce(force, i);
		}
	}
}


// ----------------------------------------------------------------------------
void PhysVehicle3D::LeftBrake(float force)
{
	for (int i = 0; i < vehicle->getNumWheels() / 2; ++i)
	{
		if(info.wheels[i].brake == true)
		{
			vehicle->setBrake(force, i);
		}
	}
}

void PhysVehicle3D::RightBrake(float force)
{
	for (int i = vehicle->getNumWheels() / 2; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].brake == true)
		{
			vehicle->setBrake(force, i);
		}
	}
}

float PhysVehicle3D::GetLeftWheelSpeed()
{
	float rotation = 0.0f;
	int nWheels = 0;
	for (int i = 1; i < vehicle->getNumWheels() / 2 - 1; ++i)
	{
			rotation += vehicle->getWheelInfo(i).m_deltaRotation;
			nWheels++;
	}
	return rotation / nWheels;
}

float PhysVehicle3D::GetRightWheelSpeed()
{
	float rotation = 0.0f;
	int nWheels = 0;
	for (int i = vehicle->getNumWheels() / 2+1; i < vehicle->getNumWheels()-1; ++i)
	{
		rotation += vehicle->getWheelInfo(i).m_deltaRotation;
		nWheels++;
	}
	return rotation / nWheels;
}

void PhysVehicle3D::RotateTurret(float amount)
{
	info.turret.horizontalJoint->enableAngularMotor(true, amount, 100000);
}
void PhysVehicle3D::RotateCanon(float amount)
{
	info.turret.verticalJoint->enableAngularMotor(true, amount, 100000);
}

void PhysVehicle3D::Fire()
{
	
}

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
float PhysVehicle3D::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}