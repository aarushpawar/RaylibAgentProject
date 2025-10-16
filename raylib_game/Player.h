#pragma once
#include "iostream"
#include "raylib.h"
#include "raymath.h"
#include "Utils.h"

class Player {
public:
    Vector2 position, lastPosition, acceleration;
    float maxSpeed, maxForce;
    Wander wanderSettings;
    Screen screen;
    Agent agent;
    bool wandering;
    int score;
    int agentType;

    Player(Vector2 position, float maxSpeed, float maxForce, Screen screen, int agentType);
    void search(Item* items, int itemCount);
    void update();
    void applyForce(Vector2 force);
    void seek(Vector2 target);
    void avoid(Vector2 target);
    void wander();
    void dash(Vector2 target);
	Vector2 calculateVelocity();
    void draw(Texture2D* textures, int frameCount);
};

