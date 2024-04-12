#include "raylib.h"
#include "Player.h"
#include "UsefulStuff.h"
#include "MapGenerator.h"

using namespace player;
int main ()
{
	InitWindow(screenWidth, screenHeight, "Tonks de game");
    MapGenerator Map(16, 8);

	SetTargetFPS(FPS);

    // Initialize player

    Player p1(StdPlayerSize, Vector2{ (float)(screenWidth / 2), (float)(screenHeight / 2) }, StdPlayerVelocity);

    // Main game cycle
    while (!WindowShouldClose())
    {
        // Here main movement logic happens
        p1.MovePlayer();

        // Drawing
        BeginDrawing();

        ClearBackground(RAYWHITE);

        for (Rectangle r : Map.map) {
            DrawRectangleRec(r, BLACK);
        }

        p1.DrawPlayer();

        if (IsKeyDown(KEY_SPACE)) {
            Map.regenerateMap();
        }

        EndDrawing();

    }


    CloseWindow();

}