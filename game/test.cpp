#include "Player.h"
#include "UsefulStuff.h"


/*
using namespace player;
int main()
{
    // Инициализируем окно

    InitWindow(screenWidth, screenHeight, "Test window");

    SetTargetFPS(FPS);

    // Инициализируем игрока

    Player p1 (StdPlayerSize, Vector2{ (float)(screenWidth / 2), (float)(screenHeight / 2) }, StdPlayerVelocity);

    // Препятствия для теста коллизий
    Rectangle obst1 = { 200, 300, 100, 300 };
    Rectangle obst2 = { 800, 100, 50, 150 };
    Rectangle obst3 = { 600, 500, 90, 210 };
    Rectangle obst4 = { 10, 0, 90, 210 };

    std::vector<Rectangle> obstacles = { obst1, obst2, obst3, obst4};

    // Главный игровой цикл
    while (!WindowShouldClose())
    {
        // Тут происходит логика движения

        p1.MovePlayer();
        
        // Тут происходит отрисовка

        BeginDrawing();

        ClearBackground(RAYWHITE);


        DrawLine(0, screenHeight / 2, screenWidth, screenHeight / 2, BLACK);
        DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, BLACK);

        for (Rectangle obst : obstacles) {
            DrawRectangleRec(obst, GRAY);
        }
        

        p1.DrawPlayer();

        EndDrawing();

    }


    CloseWindow();


    return 0;
}*/