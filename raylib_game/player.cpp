#include "raylib.h"
#include "raymath.h"
#include "Player.h"
#include "Utils.h"

Player::Player(Vector2 position, float maxSpeed, float maxForce, Screen screen, int agentType) {
    this->wandering = false;
    this->position = position;
    this->lastPosition = { position.x, position.y };
    this->acceleration = Vector2Zero();
    this->maxSpeed = maxSpeed;
    this->maxForce = maxForce;
    this->wanderSettings = { Vector2({ (float)(rand() % screen.width), (float)(rand() % screen.height) }), GetTime() + ((rand() % 1000) / 1000.f) };
    this->score = 0;
    this->agentType = agentType;

    // randomize agent type
    // 0 -> low sight range, high pick up range
    // 1 -> high sight range, low pick up range
    this->agent = (agentType % 2 == 0) ? Agent({ 0.f, (rand() % 1000) / 50.f + 50.f }) : Agent({ (rand() % 1000) / 10.f + 100.f, 0.f });
    this->screen = screen;
}

void Player::search(Item* items, int itemCount) {
    int closestItem = 0;
    float closestDistance = Vector2Distance(position, items[closestItem].position);

    for (int i = 0; i < itemCount; i++) {
        if (items[i].collected) continue;

        float distance = Vector2Distance(position, items[i].position);

        if (distance < agent.pickupRadius) {
            float speed = 0.8f;
            if (distance < agent.searchRadius && items[i].isBomb) speed *= -10;
			items[i].acceleration = Vector2Add(items[i].acceleration, Vector2Scale(Vector2Normalize(Vector2Subtract(position, items[i].position)), speed));
		}

        if (distance < closestDistance) {
            closestDistance = distance;
            closestItem = i;
        }
    }

    if (closestDistance < 30.f) {
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

    if ((GetTime() - wanderSettings.wanderTime) < .7f && (agentType == 1) && !items[closestItem].isBomb) {
        dash(items[closestItem].position);
    }

    wander();
}

Vector2 Player::calculateVelocity() {
    return Vector2Subtract(position, lastPosition);
}

void Player::update() {
    Vector2 tempPosition = { position.x, position.y };

    Vector2 velocity = Vector2Add(calculateVelocity(), acceleration);
    if (Vector2Length(velocity) > maxSpeed) {
        Vector2Scale(Vector2Normalize(velocity), maxSpeed);
    }

    position = Vector2Add(position, velocity);
    lastPosition = tempPosition;

    float hitboxSize = 12.f;
    if (position.x + hitboxSize > screen.width) position.x = screen.width - hitboxSize;
    if (position.x - hitboxSize < 0) position.x = 0 + hitboxSize;
    if (position.y + hitboxSize > screen.height) position.y = screen.height - hitboxSize;
    if (position.y - hitboxSize < 0) position.y = 0 + hitboxSize;
    acceleration = Vector2Zero();
}

void Player::applyForce(Vector2 force) {
    acceleration = Vector2Add(acceleration, force);
}

void Player::seek(Vector2 target) {
    wandering = false;
    Vector2 desired = Vector2Scale(Vector2Normalize(Vector2Subtract(target, position)), maxSpeed);
    Vector2 steer = Vector2Subtract(desired, calculateVelocity());

    // target
    DrawLine(position.x, position.y, target.x, target.y, GREEN);

    this->applyForce(Vector2ClampValue(steer, -maxForce, maxForce));
}

void Player::avoid(Vector2 target) {
    wandering = false;
    Vector2 desired = Vector2Scale(Vector2Normalize(Vector2Subtract(position, target)), maxSpeed);
    Vector2 steer = Vector2Subtract(desired, calculateVelocity());

    // target
    DrawLine(position.x, position.y, target.x, target.y, GREEN);

    this->applyForce(Vector2ClampValue(steer, -maxForce, maxForce));
}


void Player::wander() {
    if (GetTime() - wanderSettings.wanderTime >= 2) {
        wanderSettings.wanderPosition = Vector2({ (float)(rand() % screen.width), (float)(rand() % screen.height) });
        wanderSettings.wanderTime = GetTime();
    }

    if (agentType == 1) maxSpeed /= 2.5;
    seek(wanderSettings.wanderPosition);
    if (agentType == 1) maxSpeed *= 2.5;
    wandering = true;
}

void Player::dash(Vector2 target) {
    maxForce *= 20;
    seek(target);
    maxForce /= 20;
}

void Player::draw(Texture2D* textures, int frameCount) {
	Texture2D texture = textures[2];

    if (agent.searchRadius > agent.pickupRadius * 3) texture = textures[1];
	if (agent.pickupRadius > agent.searchRadius * 3) texture = textures[0];

    Vector2 size = { (float)texture.width, (float)texture.height };
    Rectangle sourceRec = { 0.0f, 0.0f, size.x, size.y };
    Rectangle destRec = { position.x, position.y, size.x, size.y };
    Vector2 origin = { size.x / 2, size.y / 2 };
    Vector2 velocity = calculateVelocity();
    float rotation = atan2(velocity.x, -velocity.y);

    // ship
    DrawTexturePro(texture, sourceRec, destRec, origin, rotation * ((float)180 / PI), WHITE);

    // ranges
	if (agent.pickupRadius == agent.searchRadius) agent.pickupRadius -= 1.f; // prevent circle overlap bug
    DrawCircleLines(position.x, position.y, agent.pickupRadius, RED); // red = pickup
    DrawCircleLines(position.x, position.y, agent.searchRadius, BLUE); // blue = search

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