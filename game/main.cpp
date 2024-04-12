#include "raylib.h"
#include "MapGenerator.h"

MapGenerator globalMapGenerator(20, 10);

const int FPS = 60;
int main ()
{
	InitWindow(screenWidth, screenHeight, "Tonks de game");

	SetTargetFPS(60);

    // Главный игровой цикл
    while (!WindowShouldClose())
    {
        // Тут происходит отрисовка
        BeginDrawing();

        ClearBackground(RAYWHITE);

        for (Rectangle r : globalMapGenerator.RealMap) {
            DrawRectangleRec(r, BLACK);
        }

        EndDrawing();

    }


    CloseWindow();

}