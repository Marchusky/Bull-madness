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
	Primitive* furniture;																			//Multiuse pointer that is used in the SetCube/Sphere/Cylinder methods.																		

	void AddPrimitive(Primitive* p);																//Adds a primitive to the primitives dynArray.
	void DeletePrimitive(Primitive* p);																//Deletes a primitive from the primitives dynArray.

	void LoadArena();																				//Loads all Arena elements. Done for readability.
	void SetCube(const vec3& position, const vec3& size, float mass, float angle, const vec3& axis, bool is_sensor = false, bool is_environment = false);				//Creates and sets a cube on the world. Used to set Arena elements.
	void SetSphere(const vec3& position, float radius, float mass, bool is_sensor = false, bool is_environment = false);												//Creates and sets a sphere on the world. Used to set Arena elements.
	void SetCylinder(const vec3& position, float radius, float height, float mass, float angle, const vec3& axis, bool is_sensor = false, bool is_environment = false);	//Creates and sets a sphere on the world. Used to set Arena elements.

	void CheckWins();
	void RestartGame();																				//Resets everyithing except Arena elements.

	//Constrains
	Cube* onetwo;
	Cube* twotwo;
	Cube* threetwo;
	Cube* fourtwo;
	Cube* fivetwo;
	Cube* sixtwo;

private:
	void HandleDebugInput();																		//Method that holds all the inputs that are activated when in debug mode.
	void DebugSpawnPrimitive(Primitive* p);															//Spawns an element on the world. Used in debug mode.

	p2DynArray<Primitive*> primitives;																//Array containing all elements that are currently existing in the world.
	p2DynArray<Primitive*> arena_elements;
	p2DynArray<Primitive*> torque_elements;
};
