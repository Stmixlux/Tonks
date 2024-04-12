#include "raylib.h"
#include "Player.h"
#include "UsefulStuff.h"
#include "MapGenerator.h"

MapGenerator globalMapGenerator(16, 8);

using namespace player;
int main ()
{
	InitWindow(screenWidth, screenHeight, "Tonks de game");

	SetTargetFPS(FPS);

    // Инициализируем игрока

    Player p1(StdPlayerSize, Vector2{ (float)(screenWidth / 2), (float)(screenHeight / 2) }, StdPlayerVelocity);

    // Главный игровой цикл
    while (!WindowShouldClose())
    {
        // Тут происходит логика движения
        p1.MovePlayer();

        // Тут происходит отрисовка
        BeginDrawing();

        ClearBackground(RAYWHITE);

        for (Rectangle r : globalMapGenerator.RealMap) {
            DrawRectangleRec(r, BLACK);
        }

        p1.DrawPlayer();

        EndDrawing();

    }


    CloseWindow();

}