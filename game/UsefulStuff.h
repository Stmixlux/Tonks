#pragma once
#include "raylib.h"
#include <deque>

// This is file with useful functions/constants that are widely used

#define DARKRED        CLITERAL(Color){ 153, 27, 37, 255 }     // Dark Red

//Map parametrs
const int XCellCount = 16;
const int YCellCount = 8;
const int screenWidth = 1280;
const int screenHeight = 720;

const int FPS = 60;

// Standart player parameters
const Vector2 StdPlayerSize = { 30, 60 };
const Vector2 StdPlayerVelocity = { 0.0f, -4.0f };
const double RotationSpeed = 0.07;

// Standart bullet parameters
const int StdBulletRadius = 4;

// Helpful operators and functions for Vector2
void operator+=(Vector2& a, const Vector2& b);
Vector2 operator+(const Vector2& a, const Vector2& b);
Vector2 operator-(const Vector2& a, const Vector2& b);
void operator-=(Vector2& a, const Vector2& b);
Vector2 operator*(const Vector2& v, float ratio);
double VectorProd(const Vector2& v1, const Vector2& v2);
double GetDistance(const Vector2& v1, const Vector2& v2);

void RotateVector2(Vector2& cent, Vector2& v, double angle);
Vector2 GetRotatedVector(Vector2& cent, Vector2& v, double angle);

// Auxiliary functions for collisions

