#pragma once
#include <raylib.h>

struct Wander {
	Vector2 wanderPosition;
	double wanderTime;
};

void Player::wander(Wander wander, int width, int height) {
    seek(wander.wanderPosition);

    if (GetTime() - wander.wanderTime >= 2) {
        wander.wanderPosition = Vector2({ (float)(rand() % width), (float)(rand() % height) });
    }
}