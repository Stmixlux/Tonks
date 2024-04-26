#include "UsefulStuff.h"
#include <math.h>

void operator+=(Vector2& a, const Vector2& b) {
	a.x += b.x;
	a.y += b.y;
}

Vector2 operator+(const Vector2& a, const Vector2& b) {
	return Vector2{ a.x + b.x, a.y + b.y };
}

Vector2 operator-(const Vector2& a, const Vector2& b) {
	return Vector2{ a.x - b.x, a.y - b.y };
}

void operator-=(Vector2& a, const Vector2& b) {
	a.x -= b.x;
	a.y -= b.y;
}

Vector2 operator*(const Vector2& v, float ratio) {
	return Vector2{ v.x * ratio, v.y * ratio };
}

double VectorProd(const Vector2& v1, const Vector2& v2)
{
	return v1.x * v2.y - v2.x * v1.y;
}

double GetDistance(const Vector2& v1, const Vector2& v2)
{
	return pow(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2), 0.5);
}

double GetAngle2Vectors(const Vector2& v1, const Vector2& v2)
{
	return acos((v1.x*v2.x + v1.y*v2.y)/GetLen(v1)/GetLen(v2));
}

double GetLen(const Vector2& v)
{
	return sqrt(pow(v.x, 2) + pow(v.y, 2));
}


void RotateVector2(Vector2& cent, Vector2& v, double angle) {
	double old_x = v.x;
	double old_y = v.y;
	v.x = cent.x + (old_x - cent.x) * cos(angle) - (old_y - cent.y) * sin(angle);
	v.y = cent.y + (old_x - cent.x) * sin(angle) + (old_y - cent.y) * cos(angle);
}

Vector2 GetRotatedVector(Vector2& cent, Vector2& v, double angle) {
	RotateVector2(cent, v, angle);
	return v;
}

bool DumbCheck(const Rectangle& rect, const Vector2& v)
{
	return (abs(rect.x - v.x) + abs(rect.x + rect.width - v.x) <= rect.width &&
		abs(rect.y - v.y) + abs(rect.y + rect.height - v.y) <= rect.height);;
}

void loadAllSounds()
{
	soundBoard[SoundBulletBurst] = LoadSound("resources/burst.mp3");
	soundBoard[SoundPlayerShoot] = LoadSound("resources/shot.mp3");
}
