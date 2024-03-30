#include "Player.h"
#include "UsefulStuff.h"


using namespace player;
int main()
{
    // Инициализируем окно

    InitWindow(screenWidth, screenHeight, "Test window");

    SetTargetFPS(FPS);

    // Инициализируем игрока

    Player p1 (Vector2{ 30, 60 }, Vector2{ (float)(screenWidth / 2), (float)(screenHeight / 2) }, Vector2{ 0.0f, -4.0f });


    // Главный игровой цикл
    while (!WindowShouldClose())
    {
        // Тут происходит логика движения

        p1.MovePlayer();
        
        // Тут происходит отрисовка

        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("move the tank with arrow keys", 10, 10, 20, DARKGRAY);
        DrawLine(0, screenHeight / 2, screenWidth, screenHeight / 2, BLACK);
        DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, BLACK);


        p1.DrawPlayer();

        EndDrawing();

    }


    CloseWindow();


    return 0;
}