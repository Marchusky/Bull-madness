#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	turn = acceleration = brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");
	Points = -1;
	HighScore = 0;
	round = 0;
	VehicleInfo car;

	// Car properties ----------------------------------------
	car.chassis_size.Set(2, 1.5, 3);
	car.chassis_offset.Set(0, 1.5, 0);
	car.mass = 500.0f;
	car.suspensionStiffness = 15.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.6f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 1.2f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;

	vec3 direction(0, -1, 0);
	vec3 axis(-1, 0, 0);

	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(88, 3, 88);	 // Byll position at start of game

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	turn = acceleration = brake = 0.0f;

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		acceleration = MAX_ACCELERATION;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
	{
		acceleration = MAX_ACCELERATION * 2;
	} 

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if (turn < TURN_DEGREES)
			turn += TURN_DEGREES;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (turn > -TURN_DEGREES)
			turn -= TURN_DEGREES;
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		brake = BRAKE_POWER;
	}

	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

	vehicle->Render();

	time = 60 - App->scene_intro->timer;
	sprintf_s(title, "%d Time Left ------ you have: %d Points-------------------Highscore: %d Points",time, Points, HighScore);
	App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}

void ModulePlayer::RestartPlayer(vec3 respawnPosition)
{
	vehicle->GetBody()->clearForces();											
	vehicle->vehicle->getRigidBody()->setLinearVelocity(btVector3(0, 0, 0));		
	vehicle->vehicle->getRigidBody()->setAngularVelocity(btVector3(0, 0, 0));

	vehicle->ResetTransform();													
	vehicle->SetPos(respawnPosition.x, respawnPosition.y, respawnPosition.z);	

}

void ModulePlayer::OnCollision(PhysBody3D * body1, PhysBody3D * body2)
{
	Color color = Color((float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f);

	/*if (body1->parentPrimitive != nullptr && body2->GetBody() == vehicle->GetBody())
	{
		body1->parentPrimitive->color = Black;
		Points++;
	}*/



	if (body2->GetBody() == vehicle->GetBody())
	{
		/*if (body1->sensor == true)
		{
			Points++;

			return;
		}*/

		for (int i = 0; i < MAX_BODIES; i++)
		{
			if (ObstacleHitted[i] == body1)
			{
				break;
			}

			if (ObstacleHitted[i] == NULL)
			{
				Points++;
				ObstacleHitted[i] = body1;
				int i = 0;
				body1->parentPrimitive->color = Black;
				if(round == i)
				{
					body1->parentPrimitive->color = Black;
				}
				else if (round == i++)
				{
					body1->parentPrimitive->color = White;
				}
				else if (round == i++)
				{
					body1->parentPrimitive->color = Purple;
				}
				else if (round == i++)
				{
					body1->parentPrimitive->color = Pink;
				}
				else if (round == i++)
				{
					body1->parentPrimitive->color = Yellow;
				}
				else if (round == i++)
				{
					body1->parentPrimitive->color = Grey;
				}
				break;
			}

			if (ObstacleHitted[MAX_BODIES - 1] != NULL)
			{
				for (int i = 0; i < MAX_BODIES; i++)
				{
					ObstacleHitted[i] = NULL;
				}
			}
		}
	}
}