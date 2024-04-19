#include "Player.h"
#include "UsefulStuff.h"

using namespace player;
Player::Player(Vector2 PS, Vector2 PP, Vector2 PV)
{
    PlayerSize = PS;
    PlayerPosition = PP;
    PlayerVelocity = PV;
    PlayerRect = { PlayerPosition.x, PlayerPosition.y, PlayerSize.x, PlayerSize.y };
    PlayerPoints.push_back(PlayerPosition + Vector2{ -0.5f * PlayerSize.x, -0.5f * PlayerSize.y });
    PlayerPoints.push_back(PlayerPosition + Vector2{ 0.5f * PlayerSize.x, -0.5f * PlayerSize.y });
    PlayerPoints.push_back(PlayerPosition + Vector2{ 0.5f * PlayerSize.x, 0.5f * PlayerSize.y });
    PlayerPoints.push_back(PlayerPosition + Vector2{ -0.5f * PlayerSize.x, 0.5f * PlayerSize.y });
}

void Player::UpdatePoints()
{
    PlayerPoints[0] = PlayerPosition + Vector2{ -0.5f * PlayerSize.x, -0.5f * PlayerSize.y };
    PlayerPoints[1] = PlayerPosition + Vector2{ 0.5f * PlayerSize.x, -0.5f * PlayerSize.y };
    PlayerPoints[2] = PlayerPosition + Vector2{ 0.5f * PlayerSize.x, 0.5f * PlayerSize.y };
    PlayerPoints[3] = PlayerPosition + Vector2{ -0.5f * PlayerSize.x, 0.5f * PlayerSize.y };
}

void Player::MovePlayer()
{
    RotationDirection = 1;
    MovingDirection = 1;
    if (IsKeyDown(KEY_UP)) {
        PlayerPosition += PlayerVelocity;
    }
    if (IsKeyDown(KEY_DOWN)) {
        MovingDirection = -1;
        PlayerPosition -= PlayerVelocity;
        RotationDirection = -1;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        PlayerAngle += RotationSpeed * RotationDirection;
        RotateVector2(Vector2{}, PlayerVelocity, RotationSpeed * RotationDirection);
    }
    if (IsKeyDown(KEY_LEFT)) {
        PlayerAngle -= RotationSpeed * RotationDirection;
        RotateVector2(Vector2{}, PlayerVelocity, -RotationSpeed * RotationDirection);
    }
    PlayerRect = { PlayerPosition.x, PlayerPosition.y, PlayerSize.x, PlayerSize.y };
    UpdatePoints();
}

void Player::DrawPlayer()
{
    DrawRectanglePro(PlayerRect, Vector2{ (float)(PlayerSize.x / 2) , (float)(PlayerSize.y / 2) }, PlayerAngle * 180 / PI, RED);
    DrawLineEx(PlayerPosition, PlayerPosition + (PlayerVelocity * 10), 3, BLACK);
    DrawCircleV(PlayerPosition, 10, DARKRED);
    for (Vector2 point : PlayerPoints) {
        DrawCircleV(GetRotatedVector(PlayerPosition, point, PlayerAngle), 3, BLACK);
    }
}

void Player::Collide(Rectangle rect) {
    float left = rect.x;
    float right = rect.x + rect.width;
    float up = rect.y;
    float down = rect.y + rect.height;
    
    for (int i = 0; i < 4; i++) {
        while (left < PlayerPoints[i].x && right > PlayerPoints[i].x && up < PlayerPoints[i].y && down > PlayerPoints[i].y) {
            PlayerPosition -= PlayerVelocity * (0.01 * MovingDirection);
            PlayerRect = { PlayerPosition.x, PlayerPosition.y, PlayerSize.x, PlayerSize.y };
            UpdatePoints();
        }
    }
    
}

void Player::Shoot()
{
    const int StdReloadTime = 30;
    if (AvailableShots == 5) {
        ReloadTime = StdReloadTime;
    }
    else {
        ReloadTime -= 1;
        if (ReloadTime == 0) {
            AvailableShots += 1;
            ReloadTime = StdReloadTime;
        }
    }

    if (InBetweenReloadTimer != 0) {
        InBetweenReloadTimer -= 1;
    }
    if (InBetweenReloadTimer == 0 && AvailableShots != 0 && IsKeyPressed(KEY_SPACE)) {
        UltimateBulletVector.push_back(Bullet{ PlayerPosition + PlayerVelocity * 10, PlayerVelocity * 2, StdBulletRadius });
        InBetweenReloadTimer = 4;
        AvailableShots -= 1;
        ReloadTime = StdReloadTime;
    }
}
