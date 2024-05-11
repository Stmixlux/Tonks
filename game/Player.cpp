#include "Player.h"
#include "UsefulStuff.h"

using namespace player;
Player::Player(Vector2 PS, Vector2 PP, Vector2 PV, int ID)
{
    PlayerSize = PS;
    PlayerPosition = PP;
    PlayerVelocity = PV;
    PlayerColor = GRAY;
    TurretColor = GRAY;
    PlayerID = ID;
    if (ID == 1) {
        PlayerColor = RED;
        TurretColor = DARKRED;
    }
    else if (ID == 2) {
        PlayerColor = BLUE;
        TurretColor = DARKBLUE;
    }
    
    PlayerRect = { PlayerPosition.x, PlayerPosition.y, PlayerSize.x, PlayerSize.y };
    PlayerPoints.push_back(PlayerPosition + Vector2{ -0.5f * PlayerSize.x, -0.5f * PlayerSize.y });
    PlayerPoints.push_back(PlayerPosition + Vector2{ 0.5f * PlayerSize.x, -0.5f * PlayerSize.y });
    PlayerPoints.push_back(PlayerPosition + Vector2{ 0.5f * PlayerSize.x, 0.5f * PlayerSize.y });
    PlayerPoints.push_back(PlayerPosition + Vector2{ -0.5f * PlayerSize.x, 0.5f * PlayerSize.y });
}

Player::Player()
{
}

void Player::UpdatePoints()
{
    PlayerPoints[0] = PlayerPosition + Vector2{ -0.5f * PlayerSize.x, -0.5f * PlayerSize.y };
    PlayerPoints[1] = PlayerPosition + Vector2{ 0.5f * PlayerSize.x, -0.5f * PlayerSize.y };
    PlayerPoints[2] = PlayerPosition + Vector2{ 0.5f * PlayerSize.x, 0.5f * PlayerSize.y };
    PlayerPoints[3] = PlayerPosition + Vector2{ -0.5f * PlayerSize.x, 0.5f * PlayerSize.y };
}

void Player::MovePlayer(bool inputs[4], std::vector<Rectangle>& walls)
{
    IsMovingStraight = false;
    IsRotating = false;
    RotationDirection = 1;
    MovingDirection = 1;
    if (inputs[2]) {
        PlayerAngle += RotationSpeed * RotationDirection;
        RotateVector2(Vector2{}, PlayerVelocity, RotationSpeed * RotationDirection);
        IsRotating = true;
    }
    if (inputs[3]) {
        RotationDirection *= -1;
        PlayerAngle += RotationSpeed * RotationDirection;
        RotateVector2(Vector2{}, PlayerVelocity, RotationSpeed * RotationDirection);
        IsRotating = true;
    }
    if (IsRotating) {
        for (Rectangle wall : walls) {
            CollideWall(wall);
        }
    }
    IsRotating = false;

    if (inputs[0]) {
        PlayerPosition += PlayerVelocity;
        IsMovingStraight = true;
    }
    if (inputs[1]) {
        MovingDirection = -1;
        PlayerPosition -= PlayerVelocity;
        RotationDirection = -1;
        IsMovingStraight = true;
    }
    PlayerRect = { PlayerPosition.x, PlayerPosition.y, PlayerSize.x, PlayerSize.y };
    UpdatePoints();

    for (Rectangle wall : walls) {
        CollideWall(wall);
    }

}

void Player::MovePlayer(std::vector<Rectangle>& walls)
{
    bool inp[4]{ IsKeyDown(KEY_UP), IsKeyDown(KEY_DOWN), IsKeyDown(KEY_RIGHT), IsKeyDown(KEY_LEFT) };
    MovePlayer(inp, walls);
}


bool Player::CheckCollisionWall(Rectangle rect) {

    Vector2 x_axis = { 1, 0 };
    Vector2 y_axis = { 0, 1 };

    Vector2 bot_left = { rect.x, rect.y + rect.height };
    Vector2 up_left = { rect.x, rect.y};
    Vector2 up_right = { rect.x + rect.width, rect.y};
    Vector2 bot_right = { rect.x + rect.width, rect.y + rect.height };

    std::vector<Vector2> RectVertexes{ bot_left, up_left, up_right, bot_right };
    std::vector<Vector2> RealPlayerVertexes;

    for (int i = 0; i < 4; i++) RealPlayerVertexes.push_back(GetRotatedVector(PlayerPosition, PlayerPoints[i], PlayerAngle));

    std::vector<Vector2> AxisForProjections = { x_axis, y_axis };
    AxisForProjections.push_back(RealPlayerVertexes[0] - RealPlayerVertexes[1]);
    AxisForProjections.push_back(RealPlayerVertexes[1] - RealPlayerVertexes[2]);
    
    bool IsColliding = true;

    for (Vector2 ax : AxisForProjections) {
        std::vector<double> PlayerProjections;
        std::vector<double> WallProjections;
        for (int i = 0; i < 4; i++) {
            PlayerProjections.push_back(GetProjection(RealPlayerVertexes[i], ax));
            WallProjections.push_back(GetProjection(RectVertexes[i], ax));
        }
        double pl_min, pl_max, w_min, w_max;
        double tpl_min, tpl_max, tw_min, tw_max;
        tpl_min = tpl_max = PlayerProjections[0];
        tw_min = tw_max = WallProjections[0];
        for (int i = 1; i < 4; i++) {
            if (PlayerProjections[i] < tpl_min) tpl_min = PlayerProjections[i];
            if (PlayerProjections[i] > tpl_max) tpl_max = PlayerProjections[i];
            if (WallProjections[i] < tw_min) tw_min = WallProjections[i];
            if (WallProjections[i] > tw_max) tw_max = WallProjections[i];
        }
        pl_min = tpl_min;
        pl_max = tpl_max;
        w_min = tw_min;
        w_max = tw_max;
        if (pl_max < w_min || w_max < pl_min) IsColliding = false;
    }
    PlayerRect = { PlayerPosition.x, PlayerPosition.y, PlayerSize.x, PlayerSize.y };
    UpdatePoints();
    return IsColliding;
}

void Player::CollideWall(Rectangle rect)
{
    if (CheckCollisionWall(rect)) {
        int counter = 0;
        if (IsRotating) {
            while (CheckCollisionWall(rect) && counter < 10) {
                PlayerAngle -= RotationSpeed * RotationDirection * 0.1;
                RotateVector2(Vector2{}, PlayerVelocity, -RotationSpeed * RotationDirection * 0.1);
                counter += 1;
            }
        }
        counter = 0;
        if (IsMovingStraight) {
            PlayerPosition -= PlayerVelocity * MovingDirection;
            PlayerRect = { PlayerPosition.x, PlayerPosition.y, PlayerSize.x, PlayerSize.y };
            UpdatePoints();

            PlayerPosition.x += PlayerVelocity.x * MovingDirection;
            if (!CheckCollisionWall(rect)) {
                PlayerPosition.x += PlayerVelocity.x * MovingDirection * (float)0.01;
            }
            while (CheckCollisionWall(rect) && counter < 10) {
                PlayerPosition.x -= PlayerVelocity.x * MovingDirection * (float)0.1;
                counter += 1;
            }

            counter = 0;
            PlayerPosition.y += PlayerVelocity.y * MovingDirection;
            if (!CheckCollisionWall(rect)) {
                PlayerPosition.y += PlayerVelocity.y * MovingDirection * (float)0.01;
            }
            while (CheckCollisionWall(rect) && counter < 10) {
                PlayerPosition.y -= PlayerVelocity.y * MovingDirection * (float)0.1;
                counter += 1;

            }
            counter = 0;
            while (CheckCollisionWall(rect) && counter < 10) {
                PlayerPosition -= PlayerVelocity * MovingDirection * (float)0.1;
                counter += 1;

            }
        }
    }
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

void Player::CollideBullet(Bullet& b)
{
    std::vector<Vector2> RealPlayerVertexes;

    for (int i = 0; i < 4; i++) RealPlayerVertexes.push_back(GetRotatedVector(PlayerPosition, PlayerPoints[i], PlayerAngle));

    std::vector<Vector2> AxisForProjections;
    AxisForProjections.push_back(RealPlayerVertexes[0] - RealPlayerVertexes[1]);
    AxisForProjections.push_back(RealPlayerVertexes[1] - RealPlayerVertexes[2]);

    bool IsColliding = true;

    for (Vector2 ax : AxisForProjections) {

        std::vector<double> PlayerProjections;
        for (int i = 0; i < 4; i++) {
            PlayerProjections.push_back(GetProjection(RealPlayerVertexes[i], ax));
        }
        double BulletProjection = GetProjection(b.Position, ax);

        double pl_min, pl_max, b_min, b_max;
        double tpl_min, tpl_max;
        tpl_min = tpl_max = PlayerProjections[0];
        for (int i = 1; i < 4; i++) {
            if (PlayerProjections[i] < tpl_min) tpl_min = PlayerProjections[i];
            if (PlayerProjections[i] > tpl_max) tpl_max = PlayerProjections[i];
        }
        pl_min = tpl_min;
        pl_max = tpl_max;
        b_min = BulletProjection - b.Radius;
        b_max = BulletProjection + b.Radius;
        if (pl_max < b_min || b_max < pl_min) IsColliding = false;
    }
    if (IsColliding) {
        IsAlive = false;
    }
}

void Player::Shoot()
{
    Shoot(IsKeyPressed(KEY_SPACE));
}

void Player::Shoot(bool isShooting)
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
    if (InBetweenReloadTimer == 0 && AvailableShots != 0 && (isShooting)) {
        UltimateBulletVector.push_back(Bullet{ PlayerPosition + PlayerVelocity * 7, PlayerVelocity * 2, StdBulletRadius });
        InBetweenReloadTimer = 4;
        AvailableShots -= 1;
        ReloadTime = StdReloadTime;
        PlaySound(soundBoard[SoundPlayerShoot]);
    }
}

bool Player::GetIsAlive()
{
    return IsAlive;
}

void Player::DrawPlayer()
{
    DrawRectanglePro(PlayerRect, Vector2{ (float)(PlayerSize.x / 2) , (float)(PlayerSize.y / 2) }, PlayerAngle * 180 / PI, PlayerColor);
    DrawLineEx(PlayerPosition, PlayerPosition + (PlayerVelocity * 8), 3, BLACK);
    DrawCircleV(PlayerPosition, 10, TurretColor);
}

std::string player::Player::toString()
{
    std::string res = boost::lexical_cast<std::string>(PlayerPosition.x) + ";" + boost::lexical_cast<std::string>(PlayerPosition.y) + ";" + boost::lexical_cast<std::string>(PlayerAngle) + ";";
    return res;
}
