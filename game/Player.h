#pragma once
#include "raylib.h"

namespace player {
	class Player
	{
	public:
		Vector2 PlayerSize;
		Vector2 PlayerPosition;
		Vector2 PlayerVelocity;
		Rectangle PlayerRect;
		double PlayerAngle = 0;
		int RotationDirection = 1;

		Player(Vector2 PS, Vector2 PP, Vector2 PV) : PlayerSize(PS), PlayerPosition(PP), PlayerVelocity(PV) {
			PlayerRect = { PlayerPosition.x, PlayerPosition.y, PlayerSize.x, PlayerSize.y };
		};
		void MovePlayer();
		void DrawPlayer();

	};
}
