#include "raylib.h"
#include "raymath.h"
#include "Player.h"
#include "Utils.h"

#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

Vector2 randomPosition(int width, int height) {
    return Vector2({ (float)(rand() % width), (float)(rand() % height) });
}

Item createRandomItem(int width, int height) {
    return Item({ false, (rand() % 4 == 0), randomPosition(width, height) });
}

Player createRandomPlayer(int width, int height, int playerType) {
    return Player(randomPosition(width, height), (float)4, (float)0.03, Screen({ width, height }), playerType);
}

void updateItems(Item items[], int n, int width, int height) {
    for (int i = 0; i < n; i++) {
        if (items[i].collected) {
			items[i] = createRandomItem(width, height);
        }
    }
}

vector<Player> newGeneration(vector<Player>& players, int width, int height) {
    std::sort(players.begin(), players.end(), [](const Player& a, const Player& b) {
        return a.score > b.score;
    });

	vector<Player> newPlayers;

    for (Player& player : players) {
		Player randomPlayer = createRandomPlayer(width, height, 0);
        randomPlayer.agent = player.agent;

		newPlayers.push_back(randomPlayer);
    }

    return newPlayers;
}

int main(void)
{
    int width = 1600, height = 800, FPS = 60;

    InitWindow(width, height, "raylib [core] example - basic window");
    SetTargetFPS(FPS);

	int playerCount = 10;
    vector<Player> players;

    for (int i = 0; i < playerCount; i++) {
		players.push_back(createRandomPlayer(width, height, i));
    }

    Texture2D textures[] = {
        LoadTexture("resources/ship.png"),
        LoadTexture("resources/ship2.png"),
        LoadTexture("resources/ship3.png"),
        LoadTexture("resources/ship4.png")
    };
    Texture2D playerTexture = LoadTexture("resources/ship.png");

    Item items[40];
	int itemCount = sizeof(items) / sizeof(items[0]);

    double lastTimestamp = GetTime();

    int frameCount = 0;

    for (int i = 0; i < itemCount; i++) {
        items[i] = createRandomItem(width, height);
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);

        frameCount++;

        for (int i = 0; i < itemCount; i++) {
            Item current = items[i];

            if (current.collected) continue;

            DrawCircle((int)current.position.x, (int)current.position.y, 4, (current.isBomb ? RED : BLACK));
        }


        for (Player& player : players) {
            player.update();
            player.search(items, itemCount);
            player.draw(textures, frameCount);
        }

		updateItems(items, itemCount, width, height);

        if (GetTime() - lastTimestamp >= 10) {
            players = newGeneration(players, width, height);
            lastTimestamp = GetTime();
		}

        EndDrawing();
    }

    CloseWindow();  

    return 0;
}

