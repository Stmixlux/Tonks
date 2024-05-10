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

		bool IsMovingStraight = false;
		bool IsRotating = false;
		int RotationDirection = 1;
		int MovingDirection = 1;

		int InBetweenReloadTimer = 0;
		int AvailableShots = 5;
		int ReloadTime = 0;

		Player(Vector2 PS, Vector2 PP, Vector2 PV);
		void MovePlayer();
		void MovePlayer(bool inputs[4]);
		void DrawPlayer();
		void UpdatePoints();
		void CollideWall(Rectangle);
		bool CollidePoint(const Vector2&, const Vector2&);
		void Shoot(bool isShooting=false);

		std::string toString();
	};
}

extern Sound soundBoard[100];
