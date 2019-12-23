#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

class Primitive;
class PhysBody3D;
class PhysMotor3D;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2) override;

public:
	Primitive* furniture;																	

	void AddPrimitive(Primitive* p);
	void DeletePrimitive(Primitive* p);

	void LoadCircuit();																				
	void SetCube(const vec3& position, const vec3& size, float mass, float angle, const vec3& axis, bool sensor = false, bool enviroment_objects = false);
	void SetSphere(const vec3& position, float radius, float mass, bool sensor = false, bool enviroment_objects = false);
	void SetCylinder(const vec3& position, float radius, float height, float mass, float angle, const vec3& axis, bool sensor = false, bool enviroment_objects = false);

	void RestartGame();																				

	//Constrains
	Cube* onetwo;
	Cube* twotwo;
	Cube* threetwo;
	Cube* fourtwo;
	Cube* fivetwo;
	Cube* sixtwo;

	//timer
	float timer;

private:
	p2DynArray<Primitive*> primitives;
	p2DynArray<Primitive*> arena_elements;
	p2DynArray<Primitive*> torque_elements;

};
