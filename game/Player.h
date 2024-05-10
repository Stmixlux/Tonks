#pragma once
#include "raylib.h"
#include "Bullet.h"
#include <vector>
#include <iostream>

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
		Color PlayerColor;
		Color TurretColor;
		int PlayerID;
		bool IsAlive = true;

		// Parameters for movement
		bool IsMovingStraight = false;
		bool IsRotating = false;
		int RotationDirection = 1;
		int MovingDirection = 1;

		// Parameters for reload
		int InBetweenReloadTimer = 0;
		int AvailableShots = 5;
		int ReloadTime = 0;

		Player(Vector2 PS, Vector2 PP, Vector2 PV, int ID);
		Player();

		// Movement
		void MovePlayer(std::vector<Rectangle>& v);
		void MovePlayer(bool inputs[4], std::vector<Rectangle>& v);
		void UpdatePoints();

		// Collision
		bool CheckCollisionWall(Rectangle);
		void CollideWall(Rectangle);
		bool CollidePoint(const Vector2&, const Vector2&);
		void CollideBullet(Bullet& b);

		// Shooting
		void Shoot();
		void Shoot(bool isShooting);

		void DrawPlayer();

		bool GetIsAlive();

		std::string toString();
	};
}

extern Sound soundBoard[100];
