#include "raylib.h"
#include "raymath.h"
#include "Player.h"
#include "Utils.h"

Player::Player(Vector2 position, float maxSpeed, float maxForce, Screen screen, int agentType) {
    this->wandering = false;
    this->position = position;
    this->velocity = Vector2Zero();
    this->acceleration = Vector2Zero();
    this->maxSpeed = maxSpeed;
    this->maxForce = maxForce;
	this->wanderSettings = { Vector2({ (float)(rand() % screen.width), (float)(rand() % screen.height) }), GetTime() };
    this->score = 0;
    
    // randomize agent type
    // 0 -> low sight range, high pick up range
	// 1 -> high sight range, low pick up range
    this->agent = (agentType % 2 == 0) ? Agent({ 0.f, (rand() % 1000) / 50.f + 50.f }) : Agent({ (rand() % 1000) / 10.f + 100.f, 15.f});
	this->screen = screen;
}

void Player::search(Item* items, int itemCount) {
    int closestItem = 0;
	float closestDistance = Vector2Distance(position, items[closestItem].position);
    for (int i = 0; i < itemCount; i++) {
        if (items[i].collected) continue;

        float distance = Vector2Distance(position, items[i].position);

        if (distance < closestDistance) {
            closestDistance = distance;
            closestItem = i;
		}
    }

    if (closestDistance < agent.pickupRadius) {
        items[closestItem].collected = true;
        if (items[closestItem].isBomb) score--;
        else score++;
        return;
    }

    if (closestDistance < agent.searchRadius) {
        if (items[closestItem].isBomb) avoid(items[closestItem].position);
        else seek(items[closestItem].position);
        return;
    }

	wander();
}

void Player::update() {
    velocity = Vector2Add(velocity, acceleration);
    if (Vector2Length(velocity) > maxSpeed) {
        Vector2Scale(Vector2Normalize(velocity), maxSpeed);
    }
    position = Vector2Add(position, velocity);
	if (position.x > screen.width) position.x = 0;
	if (position.x < 0) position.x = screen.width;
	if (position.y > screen.height) position.y = 0;
	if (position.y < 0) position.y = screen.height;
    acceleration = Vector2Zero();
}

void Player::applyForce(Vector2 force) {
    acceleration = Vector2Add(acceleration, force);
}

void Player::seek(Vector2 target) {
    wandering = false;
    Vector2 desired = Vector2Scale(Vector2Normalize(Vector2Subtract(target, position)), maxSpeed);
    Vector2 steer = Vector2Subtract(desired, velocity);

    // target
    DrawLine(position.x, position.y, target.x, target.y, GREEN);

    this->applyForce(Vector2ClampValue(steer, -maxForce, maxForce));
}

void Player::avoid(Vector2 target) {
    wandering = false;
    Vector2 desired = Vector2Scale(Vector2Normalize(Vector2Subtract(position, target)), maxSpeed);
    Vector2 steer = Vector2Subtract(desired, velocity);

    // target
    DrawLine(position.x, position.y, target.x, target.y, GREEN);

    this->applyForce(Vector2ClampValue(steer, -maxForce, maxForce));
}


void Player::wander() {
    seek(wanderSettings.wanderPosition);
    wandering = true;

    if (GetTime() - wanderSettings.wanderTime >= 2) {
        wanderSettings.wanderPosition = Vector2({ (float)(rand() % screen.width), (float)(rand() % screen.height) });
        wanderSettings.wanderTime = GetTime();
    }
}

void Player::draw(Texture2D* textures, int frameCount) {
    Vector2 size = { (float)textures[0].width, (float)textures[0].height};
    Rectangle sourceRec = { 0.0f, 0.0f, size.x, size.y };
    Rectangle destRec = { position.x, position.y, size.x, size.y };
    Vector2 origin = { size.x / 2, size.y / 2 };
    float rotation = atan2(velocity.x, -velocity.y);

    // ship
    DrawTexturePro(textures[(frameCount / 10) % 3], sourceRec, destRec, origin, rotation * ((float)180 / PI), WHITE);

    // ranges
    DrawCircleLines(position.x, position.y, agent.pickupRadius, BLUE);
    DrawCircleLines(position.x, position.y, agent.searchRadius, RED);

    // score
    Vector2 textSize = MeasureTextEx(GetFontDefault(), TextFormat("score: %d", score), 16, 0);

    int xPosition = position.x - (textSize.x / 2);
    int yPosition = position.y - (textSize.y / 2) - 36;

    DrawText(TextFormat("score: %d", score), xPosition, yPosition, 16, BLACK);

    // target
    if (wandering) {
		DrawRectangle(wanderSettings.wanderPosition.x - 5, wanderSettings.wanderPosition.y - 5, 10, 10, GREEN);
    }

	// velocity + acceleration vectors

    float scaleValue = 10.f;
	DrawLine(position.x, position.y, position.x + (velocity.x * scaleValue), position.y + (velocity.y * scaleValue), YELLOW);
	DrawLine(position.x, position.y, position.x + (acceleration.x * scaleValue * 100), position.y + (acceleration.y * scaleValue * 100), ORANGE);
}