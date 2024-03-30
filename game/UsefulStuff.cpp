#include "UsefulStuff.h"
#include <math.h>

void operator+=(Vector2& a, const Vector2& b) {
	a.x += b.x;
	a.y += b.y;
}

Vector2 operator+(const Vector2& a, const Vector2& b) {
	return Vector2{ a.x + b.x, a.y + b.y };
}

void operator-=(Vector2& a, const Vector2& b) {
	a.x -= b.x;
	a.y -= b.y;
}

Vector2 operator*(const Vector2& v, float ratio) {
	return Vector2{ v.x * ratio, v.y * ratio };
}

void RotateVector2(Vector2& v, double angle) {
	double old_x = v.x;
	double old_y = v.y;
	v.x = old_x * cos(angle) - old_y * sin(angle);
	v.y = old_x * sin(angle) + old_y * cos(angle);

}
