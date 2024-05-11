#pragma once
#include "raylib.h"
#include <deque>
#include <vector>
#include <map>
#include <string>
#include <boost/lexical_cast.hpp>

// This is file with useful functions/constants that are widely used

#define DARKRED        CLITERAL(Color){ 153, 27, 37, 255 }     // Dark Red

//Map parametrs
const int XCellCount = 12;
const int YCellCount = 6;

const int screenWidth = 1440;
const int screenHeight = 720;
const int cellWidth = screenWidth / XCellCount;
const int cellHeight = screenHeight / YCellCount;
const std::vector<Rectangle> outerWalls{ Rectangle{-7, -1, 15, screenHeight + 2}, Rectangle{screenWidth - 8 - screenWidth / XCellCount, -1, 15, screenHeight + 2},
		Rectangle{-1, -7, screenWidth + 2, 15}, Rectangle{-1, screenHeight - 8 - screenHeight / YCellCount, screenWidth + 2, 15} };

const int FPS = 60;

// Standart player parameters
const Vector2 StdPlayerSize = { 30, 50 };
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
double GetAngle2Vectors(const Vector2& v1, const Vector2& v2);
double GetLen(const Vector2& v);
double GetProjection(const Vector2& vector, const Vector2& axis); // Gets you a projection of a vector to an axis

void RotateVector2(Vector2& cent, Vector2& v, double angle);
Vector2 GetRotatedVector(Vector2& cent, Vector2& v, double angle);

// Auxiliary functions
bool DumbCheck(const Rectangle& rect, const Vector2& v);
void DrawPlayerClient(double x, double y, double angle, int ID);


// Global resource Managment
enum SoundsId {SoundPlayerShoot, SoundBulletBurst, SoundButtonClick};
extern Sound soundBoard[100];
void loadAllSounds();
void SetSoundsVolume(bool);
void unloadAllSounds();

