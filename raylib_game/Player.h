#pragma once
#include "iostream"
#include "raylib.h"
#include "raymath.h"
#include "Wander.h"

class Player {
public:
    Vector2 position, velocity, acceleration;
    float maxSpeed, maxForce;

    Player(Vector2 position, float maxSpeed, float maxForce);
    void update();
    void applyForce(Vector2 force);
    void seek(Vector2 target);
    void wander(Wander wander, int width, int height);
};

