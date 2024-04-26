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
    IsMovingStraight = false;
    IsRotating = false;
    RotationDirection = 1;
    MovingDirection = 1;
    if (IsKeyDown(KEY_UP)) {
        PlayerPosition += PlayerVelocity;
        IsMovingStraight = true;
    }
    if (IsKeyDown(KEY_DOWN)) {
        MovingDirection = -1;
        PlayerPosition -= PlayerVelocity;
        RotationDirection = -1;
        IsMovingStraight = true;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        PlayerAngle += RotationSpeed * RotationDirection;
        RotateVector2(Vector2{}, PlayerVelocity, RotationSpeed * RotationDirection);
        IsRotating = true;
    }
    if (IsKeyDown(KEY_LEFT)) {
        RotationDirection *= -1;
        PlayerAngle += RotationSpeed * RotationDirection;
        RotateVector2(Vector2{}, PlayerVelocity, RotationSpeed * RotationDirection);
        IsRotating = true;
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

void Player::CollideWall(Rectangle rect) {

    Vector2 bot_left = { rect.x, rect.y + rect.height };
    Vector2 up_left = { rect.x, rect.y};
    Vector2 up_right = { rect.x + rect.width, rect.y};
    Vector2 bot_right = { rect.x + rect.width, rect.y + rect.height };

    std::vector<Vector2> RectVertexes{ bot_left, up_left, up_right, bot_right };

    // Case 2 - the wall is in the tank
    for (int i = 0; i < 4; i++) {
        int counter = 0;
        while (CollidePoint(RectVertexes[i], Vector2{0,0})) {
            if (counter >= 10)  break;
            if (IsMovingStraight) {
                PlayerPosition -= PlayerVelocity * MovingDirection * 0.1;

            }
            //if (IsRotating) PlayerAngle -= RotationSpeed * RotationDirection * 0.1;
            counter += 1;
        }
    }

    // Case 1 - the tank is in the wall
    /*
    auto DumbCheck = [](const Rectangle& rect, const Vector2& v) {
        return (abs(rect.x - v.x) + abs(rect.x + rect.width - v.x) <= rect.width &&
            abs(rect.y - v.y) + abs(rect.y + rect.height - v.y) <= rect.height);
        };
    for (int i = 0; i < 4; i++) {
        int counter = 0;
        if (IsRotating) {
            while (DumbCheck(rect, GetRotatedVector(PlayerPosition, PlayerPoints[i], PlayerAngle)) && counter < 10) {

                PlayerAngle -= RotationSpeed * RotationDirection;
                RotateVector2(Vector2{}, PlayerVelocity, -RotationSpeed * RotationDirection);
                counter += 1;

            }
        }
        counter = 0;
        if (IsMovingStraight) {
            while (DumbCheck(rect, GetRotatedVector(PlayerPosition, PlayerPoints[i] + Vector2{ PlayerVelocity.x, 0}, PlayerAngle)) && counter < 10) {
                PlayerPosition -= Vector2{ PlayerVelocity.x * MovingDirection * (float)0.1, 0};
                counter += 1;
            }
            counter = 0;
            while (DumbCheck(rect, GetRotatedVector(PlayerPosition, PlayerPoints[i] + Vector2{0, PlayerVelocity.y }, PlayerAngle)) && counter < 10) {
                PlayerPosition -= Vector2{0, PlayerVelocity.y * MovingDirection * (float)0.1 };
                counter += 1;
            }
        }
    }
    */
    PlayerRect = { PlayerPosition.x, PlayerPosition.y, PlayerSize.x, PlayerSize.y };
    UpdatePoints();
}

bool Player::CollidePoint(const Vector2& point, const Vector2& shift)
{
    Vector2 relative_vect = point - PlayerPosition - shift;
    double angle = GetAngle2Vectors(PlayerVelocity, relative_vect);
    double len = GetLen(relative_vect);

    if (len*cos(angle) > -(float)PlayerSize.y / 2 && len*cos(angle) < (float)PlayerSize.y / 2 && len*sin(angle) > -(float)PlayerSize.x / 2 && len*sin(angle) < (float)PlayerSize.x / 2) {
        return true;
    }
    return false;
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
        PlaySound(soundBoard[SoundPlayerShoot]);
    }
}
