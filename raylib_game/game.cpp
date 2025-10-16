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
    Vector2 pos = randomPosition(width, height);
	float angle = (float)(rand() % 360) * (PI / 180.f);
    return Item({ false, (rand() % 4 == 0), pos, pos, Vector2Scale({ cos(angle), sin(angle) }, 4.f) });
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

    for (int i = 0; i < players.size(); i++) {
        for (int j = i + 1; j < players.size(); j++) {
			Player& player = players[i];
			Player& player2 = players[j];

            if (&player == &player2) continue;
            if (newPlayers.size() >= players.size()) break;

            if (Vector2Distance(player.position, player2.position) < 200.f) {
                Player child = createRandomPlayer(width, height, 0);
                child.agent = {
                    max(player.agent.pickupRadius, player2.agent.pickupRadius),
                    max(player.agent.searchRadius, player2.agent.searchRadius)
                };
                newPlayers.push_back(child);
            }
        }
    }

    float scalingValue = 0.9f;
    int i = 0;

    for (Player& player : players) {
        if (newPlayers.size() >= players.size()) break;
        Player randomPlayer = createRandomPlayer(width, height, 0);

        randomPlayer.agent = player.agent;

        if (i < 3) {
            randomPlayer.agent.pickupRadius = (player.agentType % 2 == 0) ? player.agent.pickupRadius * 1.2 * scalingValue : player.agent.pickupRadius;
            randomPlayer.agent.searchRadius = (player.agentType % 2 == 0) ? player.agent.searchRadius : player.agent.searchRadius * 1.2 * scalingValue;
            
            i++;
        }
        else if (i < 5)
        {
            randomPlayer.agent.pickupRadius = (player.agentType % 2 == 0) ? player.agent.pickupRadius : player.agent.pickupRadius * 1.5 * scalingValue;
            randomPlayer.agent.searchRadius = (player.agentType % 2 == 0) ? player.agent.searchRadius * 1.5 * scalingValue : player.agent.searchRadius;
            i++;
        }
        else
        {
            randomPlayer.agent.pickupRadius = player.agent.pickupRadius;
            randomPlayer.agent.searchRadius = player.agent.searchRadius;
        }

        if (randomPlayer.agent.pickupRadius > 150) {
            randomPlayer.agent.pickupRadius = 150;
        }

		if (randomPlayer.agent.searchRadius > 200) {
			randomPlayer.agent.searchRadius = 200;
		}

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
        LoadTexture("resources/redship.png"),
        LoadTexture("resources/blueship.png"),
        LoadTexture("resources/redblueship.png"),
    };

    Item items[40];
	int itemCount = sizeof(items) / sizeof(items[0]);

    double lastTimestamp = GetTime();

    int frameCount = 0;
    int generation = 0;

    for (int i = 0; i < itemCount; i++) {
        items[i] = createRandomItem(width, height);
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);

        frameCount++;
        

        for (int i = 0; i < itemCount; i++) {
            if (items[i].collected) continue;

            Vector2 tempPosition = { items[i].position.x, items[i].position.y };
            Vector2 velocity = Vector2Scale(Vector2Add(Vector2Subtract(items[i].position, items[i].lastPosition), items[i].acceleration), 0.95);

            items[i].position = Vector2Add(items[i].position, velocity);
            items[i].lastPosition = tempPosition;
            items[i].acceleration = Vector2Zero();

            DrawCircle((int)items[i].position.x, (int)items[i].position.y, 4, (items[i].isBomb ? RED : BLACK));
        }

        for (Player& player : players) {
            player.update();
            player.search(items, itemCount);
            player.draw(textures, frameCount);
        }

        updateItems(items, itemCount, width, height);

        if (GetTime() - lastTimestamp >= 30) {
            players = newGeneration(players, width, height);
            lastTimestamp = GetTime();
            generation++;
		}

        Vector2 textSize = MeasureTextEx(GetFontDefault(), TextFormat("Generation #%i | Time Left: %.1fs", generation + 1, 30 - (GetTime() - lastTimestamp)), 32, 0);

        int xPosition = (width / 2.f) - (textSize.x / 2);
        int yPosition = (50.f);
        DrawText(TextFormat("Generation #%i | Time Left: %.1fs", generation + 1, 30 - (GetTime() - lastTimestamp)), xPosition, yPosition, 32, BLACK);


        EndDrawing();
    }

    CloseWindow();  

    return 0;
}

