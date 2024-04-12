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

    // �������������� ������

    Player p1(StdPlayerSize, Vector2{ (float)(screenWidth / 2), (float)(screenHeight / 2) }, StdPlayerVelocity);

    // ������� ������� ����
    while (!WindowShouldClose())
    {
        // ��� ���������� ������ ��������
        p1.MovePlayer();

        // ��� ���������� ���������
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