#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "ModulePlayer.h"
#include "PhysVehicle3D.h"
#include "ModuleRenderer3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
//, top_constrained_cube(nullptr)
{
	//app->renderer3D->skyBoxColor = vec3(1.f, 1.f, 1.f);		//Setting the skybox's color from ModuleSceneIntro. Color white.
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	LoadCircuit();

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	for (int i = 0; i < primitives.Count(); i++)									
	{
		DeletePrimitive(primitives[i]);
	}

	
	primitives.Clear();
	LoadCircuit();

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	Plane p(vec3(0, 1, 0));
	p.axis = true;
	p.Render();

	//control game
	if (dt == 6)
	{
		RestartGame();
	}

	if (App->player->vehicle->chassis.transform.M[13] <= -3)
	{
		App->player->RestartPlayer(vec3(88, 6, 88));
	}

	//IMPORTANTE
	//Camera Management

	//LOOK AT------------------------------------------------------------------
	float chassis_X = App->player->vehicle->chassis.transform.M[12]; //coordenada X del vehículo
	float chassis_Y = App->player->vehicle->chassis.transform.M[13]; //coordenada Y "
	float chassis_Z = App->player->vehicle->chassis.transform.M[14]; //coordenada Z "

	App->camera->LookAt(vec3(chassis_X, chassis_Y, chassis_Z));

	//DISTANCE BETWEEN CAMERA AND CAR-------------------------------------------
	float distance_X = chassis_X - App->camera->Position.x;
	float distance_Y = chassis_Y - App->camera->Position.y;
	float distance_Z = chassis_Z - App->camera->Position.z;

	float absoluteDistance_X = 0;
	float absoluteDistance_Y = 0;
	float absoluteDistance_Z = 0;

	if ((distance_X) >= 0)
		absoluteDistance_X = (distance_X);
	else
		absoluteDistance_X = (distance_X)*(-1);

	if ((distance_Y) >= 0)
		absoluteDistance_Y = (distance_Y);
	else
		absoluteDistance_Y = (distance_Y)*(-1);

	if ((distance_Z) >= 0)
		absoluteDistance_Z = (distance_Z);
	else
		absoluteDistance_Z = (distance_Z)*(-1);

	//CAMERA POSITION------------------------------------------------------------
	float boundary_X = 20;
	float boundary_Y = 7;
	float boundary_Z = 20;

	float movement_X = 0;
	float movement_Y = 0;
	float movement_Z = 0;

	if (absoluteDistance_X > boundary_X)
	{
		if (distance_X >= 0)
			movement_X = (distance_X - boundary_X);
		else
			movement_X = (distance_X + boundary_X);
	}

	if (absoluteDistance_Y > boundary_Y)
	{
		if (distance_Y >= 0)
			movement_Y = (distance_Y - boundary_Y);
		else
			movement_Y = (distance_Y + boundary_Y);
	}

	if (absoluteDistance_Z > boundary_Z)
	{
		if (distance_Z >= 0)
			movement_Z = (distance_Z - boundary_Z);
		else
			movement_Z = (distance_Z + boundary_Z);
	}

	App->camera->Move(vec3(movement_X, movement_Y, movement_Z));
	
	if (App->debug == true)
		HandleDebugInput();

	for (uint n = 0; n < primitives.Count(); n++)
		primitives[n]->Update();

	//Applying torque to the arena's constraints.
	onetwo->body.GetBody()->applyTorque(btVector3(0.0f, 30000.0f, 0.0f));
	twotwo->body.GetBody()->applyTorque(btVector3(0.0f, 30000.0f, 0.0f));
	threetwo->body.GetBody()->applyTorque(btVector3(0.0f, 30000.0f, 0.0f));
	fourtwo->body.GetBody()->applyTorque(btVector3(0.0f, 30000.0f, 0.0f));
	fivetwo->body.GetBody()->applyTorque(btVector3(0.0f, 30000.0f, 0.0f));
	sixtwo->body.GetBody()->applyTorque(btVector3(0.0f, 30000.0f, 0.0f));

	/*for (uint n = 0; n < arena_elements.Count(); n++)
		arena_elements[n]->Update();*/

	return UPDATE_CONTINUE;
}

update_status ModuleSceneIntro::PostUpdate(float dt)
{
	for (uint n = 0; n < primitives.Count(); n++)
	{
		primitives[n]->Render();
	}
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		RestartGame();
	}
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		App->player->RestartPlayer(vec3(88, 6, 88));
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D * body1, PhysBody3D * body2)
{
	Color color = Color((float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f);

	if (body1->parentPrimitive != nullptr)
	{
		body1->parentPrimitive->color = color;
	}
	
	if (body2->parentPrimitive != nullptr)
	{
		body2->parentPrimitive->color = color;
	}
}

void ModuleSceneIntro::HandleDebugInput()
{
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		DebugSpawnPrimitive(new Sphere());
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		DebugSpawnPrimitive(new Cube());
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
		DebugSpawnPrimitive(new Cylinder());
	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
		for (uint n = 0; n < primitives.Count(); n++)
			primitives[n]->SetPos((float)(std::rand() % 40 - 20), 10.f, (float)(std::rand() % 40 - 20));
	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
		for (uint n = 0; n < primitives.Count(); n++)
			primitives[n]->body.Push(vec3((float)(std::rand() % 500) - 250, 500, (float)(std::rand() % 500) - 250));

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		//TODO: NEW CODE
		//A snippet of new code that may be useful for you. Nothing to do here really


		//Get a vector indicating the direction from the camera viewpoint to the "mouse"
		const vec2 mousePos(((float)App->input->GetMouseX() / (float)App->window->Width()) * 2.f - 1.f,
			-((float)App->input->GetMouseY() / (float)App->window->Height()) * 2.f + 1.f);
		const vec4 rayEye = inverse(App->renderer3D->ProjectionMatrix) * vec4(mousePos.x, mousePos.y, -1.f, 1.f);
		const vec4 rayWorld(inverse(App->camera->GetViewMatrix()) * vec4(rayEye.x, rayEye.y, -1.f, 0.f));

		vec3 Dir(rayWorld.x, rayWorld.y, rayWorld.z);
		//Cast a ray from the camera, in the "mouse" direction
		PhysBody3D* body = App->physics->RayCast(App->camera->Position, Dir);
		if (body)
		{
			//Change the color of the clicked primitive
			body->parentPrimitive->color = Color((float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f);
		}
	}
}

void ModuleSceneIntro::DebugSpawnPrimitive(Primitive * p)
{
	primitives.PushBack(p);
	p->SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
	p->body.Push(-App->camera->Z * 1000.f);
}

// --- Adds a primitive to the primitives dynArray.
void ModuleSceneIntro::AddPrimitive(Primitive * p)
{
	primitives.PushBack(p);
}

void ModuleSceneIntro::DeletePrimitive(Primitive* p)
{
	for (int i = 0; i < primitives.Count(); i++)									//Revise this.
	{
		if (primitives[i] == p)
		{
			if (primitives[i]->body.is_sensor == false && primitives[i]->body.is_environment == false)
			{
				App->physics->RemoveBodyFromWorld(primitives[i]->body.GetBody());
				delete primitives[i];
				primitives.Pop(primitives[i]);
			}

			break;
		}
	}
}



void ModuleSceneIntro::LoadCircuit()
{
	// Ground
	/*SetCube(vec3(88.0f, -2.0f, 88.0f), vec3(200.f, 5.f, 200.f), 0.0f, 0, vec3(1, 0, 0), false, true);*/
	Cube* Ground = new Cube(vec3(200.f, 5.f, 200.f), 0.0f, false, true);
	Ground->SetPos(88.0f, -2.0f, 88.0f);
	primitives.PushBack(Ground);
	Ground->color = Yellow;

	// Circuit
	// 1
	SetCylinder(vec3(60.0f, 0.0f, 16.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(40.0f, 0.0f, 16.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(52.0f, 0.0f, 0.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(76.0f, 0.0f, 0.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	// 2
	SetCylinder(vec3(76.0f, 0.0f, 36.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(52.0f, 0.0f, 32.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(56.0f, 0.0f, 48.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(36.0f, 0.0f, 48.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(36.0f, 0.0f, 72.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(76.0f, 0.0f, 60.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	// 3
	SetCylinder(vec3(136.0f, 0.0f, 56.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(160.0f, 0.0f, 40.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(168.0f, 0.0f, 52.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(160.0f, 0.0f, 68.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(168.0f, 0.0f, 56.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	// 4
	SetCylinder(vec3(108.0f, 0.0f, 80.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(108.0f, 0.0f, 84.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	// 5
	SetCylinder(vec3(92.0f, 0.0f, 52.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(88.0f, 0.0f, 28.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(96.0f, 0.0f, 32.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	// 6
	SetCylinder(vec3(116.0f, 0.0f, 68.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(116.0f, 0.0f, 100.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(140.0f, 0.0f, 80.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	// 7
	SetCylinder(vec3(136.0f, 0.0f, 122.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(136.0f, 0.0f, 140.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(112.0f, 0.0f, 144.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(168.0f, 0.0f, 108.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(172.0f, 0.0f, 132.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(164.0f, 0.0f, 160.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(128.0f, 0.0f, 176.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(96.0f, 0.0f, 168.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	// 8
	SetCylinder(vec3(52.0f, 0.0f, 112.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(44.0f, 0.0f, 136.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(60.0f, 0.0f, 136.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(76.0f, 0.0f, 120.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	// 9
	SetCylinder(vec3(4.0f, 0.0f, 108.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(8.0f, 0.0f, 136.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(24.0f, 0.0f, 144.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(28.0f, 0.0f, 120.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	// 10
	SetCylinder(vec3(60.0f, 0.0f, 20.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);
	SetCylinder(vec3(36.0f, 0.0f, 72.0f), 0.5f, 10.0f, 0.0f, 90, vec3(0, 0, 1), true, true);


	// Obstacles
	// 1
	Cube* oneone = new Cube(vec3(1.0f, 1.0f, 1.0f), 0.0f, false, true);
	oneone->SetPos(80.0f, 5.0f, 20.0f);
	primitives.PushBack(oneone);
	oneone->color = Red;

	onetwo = new Cube(vec3(5.0f, 8.0f, 1.0f), 1000.0f, false, true);
	primitives.PushBack(onetwo);
	onetwo->color = Blue;

	App->physics->AddConstraintHinge(*oneone, *onetwo, vec3(0.0f, 0.0f, 0.0f), vec3(-3.0f, 0.0f, 0.0f), vec3(0, 1, 0), vec3(0, 1, 0), true);
	// 2
	Cube* twoone = new Cube(vec3(1.0f, 1.0f, 1.0f), 0.0f, false, true);
	twoone->SetPos(60.0f, 5.0f, 60.0f);
	primitives.PushBack(twoone);
	twoone->color = Red;

	twotwo = new Cube(vec3(5.0f, 8.0f, 1.0f), 1000.0f, false, true);
	primitives.PushBack(twotwo);
	twotwo->color = Blue;

	App->physics->AddConstraintHinge(*twoone, *twotwo, vec3(0.0f, 0.0f, 0.0f), vec3(-3.0f, 0.0f, 0.0f), vec3(0, 1, 0), vec3(0, 1, 0), true);
	// 3
	Cube* threeone = new Cube(vec3(1.0f, 1.0f, 1.0f), 0.0f, false, true);
	threeone->SetPos(140.0f, 5.0f, 60.0f);
	primitives.PushBack(threeone);
	threeone->color = Red;

	threetwo= new Cube(vec3(5.0f, 8.0f, 1.0f), 1000.0f, false, true);
	primitives.PushBack(threetwo);
	threetwo->color = Blue;

	App->physics->AddConstraintHinge(*threeone, *threetwo, vec3(0.0f, 0.0f, 0.0f), vec3(-3.0f, 0.0f, 0.0f), vec3(0, 1, 0), vec3(0, 1, 0), true);
	// 4 
	Cube* fourone = new Cube(vec3(1.0f, 1.0f, 1.0f), 0.0f, false, true);
	fourone->SetPos(60.0f, 5.0f, 100.0f);
	primitives.PushBack(fourone);
	fourone->color = Red;

	fourtwo= new Cube(vec3(5.0f, 8.0f, 1.0f), 1000.0f, false, true);
	primitives.PushBack(fourtwo);
	fourtwo->color = Blue;

	App->physics->AddConstraintHinge(*fourone, *fourtwo, vec3(0.0f, 0.0f, 0.0f), vec3(-3.0f, 0.0f, 0.0f), vec3(0, 1, 0), vec3(0, 1, 0), true);
	// 5
	Cube* fiveone = new Cube(vec3(1.0f, 1.0f, 1.0f), 0.0f, false, true);
	fiveone->SetPos(40.0f, 5.0f, 120.0f);
	primitives.PushBack(fiveone);
	fiveone->color = Red;

	fivetwo= new Cube(vec3(5.0f, 8.0f, 1.0f), 1000.0f, false, true);
	primitives.PushBack(fivetwo);
	fivetwo->color = Blue;

	App->physics->AddConstraintHinge(*fiveone, *fivetwo, vec3(0.0f, 0.0f, 0.0f), vec3(-3.0f, 0.0f, 0.0f), vec3(0, 1, 0), vec3(0, 1, 0), true);
	// 6
	Cube* sixone = new Cube(vec3(1.0f, 1.0f, 1.0f), 0.0f, false, true);
	sixone->SetPos(120.0f, 5.0f, 120.0f);
	primitives.PushBack(sixone);
	sixone->color = Red;

	sixtwo = new Cube(vec3(5.0f, 8.0f, 1.0f), 1000.0f, false, true);
	primitives.PushBack(sixtwo);
	sixtwo->color = Blue;

	App->physics->AddConstraintHinge(*sixone, *sixtwo, vec3(0.0f, 0.0f, 0.0f), vec3(-3.0f, 0.0f, 0.0f), vec3(0, 1, 0), vec3(0, 1, 0), true);

}

void ModuleSceneIntro::SetCube(const vec3& position, const vec3& size, float mass, float angle, const vec3& axis, bool is_sensor, bool is_environment)
{
	furniture = new Cube(size, mass, is_sensor, is_environment);						//Creates a cube primitive (and its body at Cube's class constructor).
	furniture->SetPos(position.x, position.y, position.z);								//Sets the position of the element in the world.
	primitives.PushBack(furniture);														//Adds the new element to the primitives array.

	furniture->SetRotation(angle, axis);												//Sets the amount of rotation that the element will have.

	Color color = Color((float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f);
	furniture->color = color;															//Sets the element's colour.
}

void ModuleSceneIntro::SetSphere(const vec3& position, float radius, float mass, bool is_sensor, bool is_environment)
{
	furniture = new Sphere(radius, mass, is_sensor, is_environment);					//Creates a sphere primitive (and its body at Sphere's class constructor).
	furniture->SetPos(position.x, position.y, position.z);								//Sets the position of the element in the world.
	primitives.PushBack(furniture);														//Adds the new element to the primitives array.

	furniture->color = White;															//Sets the element's colour.
}

void ModuleSceneIntro::SetCylinder(const vec3& position, float radius, float height, float mass, float angle, const vec3& axis, bool is_sensor, bool is_environment)
{
	furniture = new Cylinder(radius, height, mass, is_sensor, is_environment);			//Creates a cylinder primitive (and its body at Cylinder's class constructor).
	furniture->SetPos(position.x, position.y, position.z);								//Sets the position of the element in the world.
	primitives.PushBack(furniture);														//Adds the new element to the primitives array.

	furniture->SetRotation(angle, axis);												//Sets the amount of rotation that the element will have.

	Color color = Color((float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f);
	furniture->color = color;															//Sets the element's colour.
}

void ModuleSceneIntro::RestartGame()
{
	if (App->player->Points > App->player->HighScore)
	{
		App->player->HighScore = App->player->Points;
	}
	App->player->RestartPlayer(vec3(88, 6, 88));
	App->player->Points = 0;
}