#include "Bullet.h"

void Bullet::MoveBullet()
{
	Position += Velocity;
	LivingTime -= 1;
	if (LivingTime == 0) {
		//IsAlive = false;
		PlaySound(soundBoard[SoundBulletBurst]);
		UltimateBulletVector.pop_front();
	}
}

void Bullet::DrawBullet() 
{
	DrawCircleV(Position, Radius, BLACK);
}

void Bullet::Collide(Rectangle rect)
{

	Vector2 test_vect_x = Position + Vector2{ Velocity.x, 0 };
	Vector2 test_vect_y = Position + Vector2{ 0, Velocity.y };

	// Borders of map check
	if (test_vect_x.x >= screenWidth || test_vect_x.x <= 0) {
		Velocity.x *= -1;
	}

	if (test_vect_y.y >= screenHeight || test_vect_y.y <= 0) {
		Velocity.y *= -1;
	}

	// Dumb check for rectangles
	auto DumbCheck = [](const Rectangle& rect, const Vector2& v) {
		return (abs(rect.x - v.x) + abs(rect.x + rect.width - v.x) <= rect.width &&
			abs(rect.y - v.y) + abs(rect.y + rect.height - v.y) <= rect.height);
		};

	if (DumbCheck(rect, test_vect_x)) {
		Velocity.x *= -1;
		return;
	}
	if (DumbCheck(rect, test_vect_y)) {
		Velocity.y *= -1;
	}
	

}
