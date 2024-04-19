#pragma once
#include "raylib.h"
#include "Bullet.h"
#include <vector>

namespace player {
	class Player
	{
	public:
		Vector2 PlayerSize;
		Vector2 PlayerPosition;
		Vector2 PlayerVelocity;
		Rectangle PlayerRect;
		std::vector<Vector2> PlayerPoints;
		double PlayerAngle = 0;
		int RotationDirection = 1;
		int MovingDirection = 1;

		int InBetweenReloadTimer = 0;
		int AvailableShots = 5;
		int ReloadTime = 0;

		Player(Vector2 PS, Vector2 PP, Vector2 PV);
		void MovePlayer();
		void DrawPlayer();
		void UpdatePoints();
		void Collide(Rectangle);
		void Shoot();
	};
}
