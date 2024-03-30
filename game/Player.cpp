#include "Player.h"
#include "UsefulStuff.h"

using namespace player;
void Player::MovePlayer()
{
    if (IsKeyDown(KEY_UP)) {
        PlayerPosition += PlayerVelocity;
        RotationDirection = 1;
    }
    if (IsKeyDown(KEY_DOWN)) {
        PlayerPosition -= PlayerVelocity;
        RotationDirection = -1;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        PlayerAngle += RotationSpeed * RotationDirection;
        RotateVector2(PlayerVelocity, RotationSpeed * RotationDirection);
    }
    if (IsKeyDown(KEY_LEFT)) {
        PlayerAngle -= RotationSpeed * RotationDirection;
        RotateVector2(PlayerVelocity, -RotationSpeed * RotationDirection);
    }
    PlayerRect = { PlayerPosition.x, PlayerPosition.y, PlayerSize.x, PlayerSize.y };
}

void Player::DrawPlayer()
{
    DrawRectanglePro(PlayerRect, Vector2{ (float)(PlayerSize.x / 2) , (float)(PlayerSize.y / 2) }, PlayerAngle * 180 / PI, RED);
    DrawLineEx(PlayerPosition, PlayerPosition + (PlayerVelocity * 10), 3, BLACK);
    DrawCircleV(PlayerPosition, 10, DARKRED);
}
