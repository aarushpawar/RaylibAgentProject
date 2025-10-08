#include "raylib.h"
#include "raymath.h"
#include "Player.h"
#include "Item.h"

#include <iostream>

using namespace std;

Item createRandomItem(int width, int height) {
    return Item({ false, { (float)(rand() % width), (float)(rand() % height) } });
}

void selectRandomItem(Item items[], int n) {
    for (int i = 0; i < n; i++) {
        items[i].selected = false;
    }

    int index = (int)floor(rand() % n);
    items[index].selected = true;
}

int main(void)
{
    int width = 1600, height = 800, FPS = 60;

    InitWindow(width, height, "raylib [core] example - basic window");
    SetTargetFPS(FPS);

    Player player = Player(Vector2Zero(), (float)4, (float)0.03);
    Texture2D textures[] = {
        LoadTexture("resources/ship.png"),
        LoadTexture("resources/ship2.png"),
        LoadTexture("resources/ship3.png"),
        LoadTexture("resources/ship4.png")
    };
    Texture2D playerTexture = LoadTexture("resources/ship.png");

    int itemCount = 10;
    Item items[10];

    double lastTimestamp = GetTime();

    int frameCount = 0;

    for (int i = 0; i < itemCount; i++) {
        items[i] = createRandomItem(width, height);
    }

    selectRandomItem(items, itemCount);
    
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);

        frameCount++;

        for (int i = 0; i < itemCount; i++) {
            Item current = items[i];
            DrawCircle((int)current.position.x, (int)current.position.y, 8, (current.selected ? RED : BLACK));
            
            if (!current.selected) continue;

            player.seek(current.position);
        }

        if (GetTime() - lastTimestamp > 5) {
            selectRandomItem(items, itemCount);
            lastTimestamp = GetTime();
        }

        player.update();
        //DrawCircle(player.position.x, player.position.y, 24, BLACK);

        Vector2 size = { (float)playerTexture.width, (float)playerTexture.height };
        Rectangle sourceRec = { 0.0f, 0.0f, size.x, size.y };
        Rectangle destRec = { player.position.x, player.position.y, size.x, size.y };
        Vector2 origin = { size.x / 2, size.y / 2 };
        float rotation = atan2(player.velocity.x, -player.velocity.y);

        DrawTexturePro(textures[(frameCount / 10) % 3], sourceRec, destRec, origin, rotation * ((float)180 / PI), WHITE);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

