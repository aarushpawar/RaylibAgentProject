#include "raylib.h"
#include "raymath.h"
#include "Player.h"
#include "Item.h"
#include "Wander.h"

Player::Player(Vector2 position, float maxSpeed, float maxForce) {
    this->position = position;
    this->velocity = Vector2Zero();
    this->acceleration = Vector2Zero();
    this->maxSpeed = maxSpeed;
    this->maxForce = maxForce;
}

void Player::update() {
    velocity = Vector2Add(velocity, acceleration);
    if (Vector2Length(velocity) > maxSpeed) {
        Vector2Scale(Vector2Normalize(velocity), maxSpeed);
    }
    position = Vector2Add(position, velocity);
    acceleration = Vector2Zero();
}

void Player::applyForce(Vector2 force) {
    acceleration = Vector2Add(acceleration, force);
}

void Player::seek(Vector2 target) {
    Vector2 desired = Vector2Scale(Vector2Normalize(Vector2Subtract(target, position)), maxSpeed);
    Vector2 steer = Vector2Subtract(desired, velocity);

    this->applyForce(Vector2ClampValue(steer, -maxForce, maxForce));
}

