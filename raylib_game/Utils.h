#pragma once
#include "raylib.h"
#include "raymath.h"

struct Item {
	bool collected = false;
	bool isBomb = false;
	Vector2 position;
	Vector2 lastPosition;
	Vector2 acceleration;
};

struct Wander {
	Vector2 wanderPosition;
	double wanderTime;
};

struct Agent {
	float searchRadius;
	float pickupRadius;
};

struct Screen {
	int width;
	int height;
};;