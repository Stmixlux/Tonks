#pragma once
#include "raylib.h"

// Это файл с полезными функциями/константами, которые много где используются

#define DARKRED        CLITERAL(Color){ 153, 27, 37, 255 }     // Dark Red

const int screenWidth = 1280;
const int screenHeight = 720;

const int FPS = 60;

const double RotationSpeed = 0.07;

void operator+=(Vector2& a, const Vector2& b);
Vector2 operator+(const Vector2& a, const Vector2& b);
void operator-=(Vector2& a, const Vector2& b);
Vector2 operator*(const Vector2& v, float ratio);
void RotateVector2(Vector2& v, double angle);
