#pragma once
#include "raylib.h"
#include <deque>
#include <vector>


class Bullet
{
public:
	Vector2 Position;
	Vector2 Velocity;
	int Radius;
	int LivingTime = 600;
	//bool IsAlive = true;


	Bullet(Vector2 P, Vector2 V, int r) : Position(P), Velocity(V), Radius(r) {};
	//~Bullet();
	void MoveBullet();
	void DrawBullet();
	void Collide(Rectangle rect);

};

extern std::deque<Bullet> UltimateBulletVector;