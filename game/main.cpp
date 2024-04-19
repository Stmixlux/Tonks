#include "raylib.h"
#include "Player.h"
#include "UsefulStuff.h"
#include "MapGenerator.h"

std::deque<Bullet> UltimateBulletVector;


/* TODO:
    Fix bullet death    (get rid of memory leaks)
    Implement collision for tank (hard :( )

*/

using namespace player;
int main ()
{
	InitWindow(screenWidth, screenHeight, "Tonks de game");
    MapGenerator Map(16, 8);

	SetTargetFPS(FPS);

    // Player initialization

    Player p1(StdPlayerSize, Vector2{ (float)(screenWidth / 2), (float)(screenHeight / 2) }, StdPlayerVelocity);


    // Main game cycle
    while (!WindowShouldClose())
    {
        // Here happens moving logic
        for (int i = 0; i < UltimateBulletVector.size(); i++) {
            for (Rectangle rect : globalMapGenerator.RealMap) {
                UltimateBulletVector[i].Collide(rect);
            }
        }


        p1.MovePlayer();


        for (int i = 0; i < UltimateBulletVector.size(); i++) {
            UltimateBulletVector[i].MoveBullet();
        }
        


        // Here happens shooting

        p1.Shoot();

        // Here happens drawing
        BeginDrawing();

        ClearBackground(RAYWHITE);

        for (Rectangle r : Map.map) {
            DrawRectangleRec(r, BLACK);
        }

        p1.DrawPlayer();

        if (IsKeyDown(KEY_SPACE)) {
            Map.regenerateMap();
        }


        for (int i = 0; i < UltimateBulletVector.size(); i++) {
            UltimateBulletVector[i].DrawBullet();
        }



        EndDrawing();

    }


    CloseWindow();

}