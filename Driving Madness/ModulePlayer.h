#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

struct PhysVehicle3D;

#define MAX_ACCELERATION 1000.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define BRAKE_POWER 1000.0f

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void RestartPlayer(vec3 respawnPosition);

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2) override;

public:

	PhysVehicle3D* vehicle;
	float turn;
	float acceleration;
	float brake;
	int	  Points;
	int HighScore;
	PhysBody3D* prevCollBody[MAX_BODIES];


};