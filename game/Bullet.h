#pragma once
#include "raylib.h"
#include "UsefulStuff.h"
#include <deque>
#include <vector>
#include "UsefulStuff.h"


class Bullet
{
public:
	Vector2 Position;
	Vector2 Velocity;
	int Radius;
	int LivingTime = 600;


	Bullet(Vector2 P, Vector2 V, int r) : Position(P), Velocity(V), Radius(r) {};
	void MoveBullet();
	void DrawBullet();
	void Collide(Rectangle rect);

	std::string toString();
};

extern std::deque<Bullet> UltimateBulletVector;
extern Sound soundBoard[100];