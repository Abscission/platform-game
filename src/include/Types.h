
//Copyright (c) 2015 Jason Light
//License: MIT

#pragma once

#include <cinttypes>

typedef uint8_t byte;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef float float32;
typedef double float64;

#define KILOBYTES(n) (n * 1024)
#define MEGABYTES(n) (KILOBYTES(n) * 1024)
#define GIGABYTES(n) (MEGABYTES(n) * 1024)

struct IVec2 {
	int X;
	int Y;

	inline IVec2 operator+(const IVec2& b) {
		IVec2 a = *this;
		a.X += b.X;
		a.Y += b.Y;
		return a;
	}

	inline IVec2 operator-(const IVec2& b) {
		IVec2 a = *this;
		a.X -= b.X;
		a.Y -= b.Y;
		return a;
	}
};

struct V2_16 {
	int16 X;
	int16 Y;
};

struct Vector2 {
	float X;
	float Y;

	//Overload the == operator so Vector2s can be checked for equality
	inline bool operator==(Vector2 a) {
		if (a.X == this->X && a.Y == this->Y) {
			return true;
		}
		else {
			return false;
		}
	}

	inline Vector2 operator+(const Vector2& b) {
		Vector2 a = *this;
		a.X += b.X;
		a.Y += b.Y;
		return a;
	}

	inline Vector2 operator*(const int b){
		Vector2 a = *this;
		a.X *= b;
		a.Y *= b;
		return a;
	}

	inline Vector2 operator*(const float b){
		Vector2 a = *this;
		a.X *= b;
		a.Y *= b;
		return a;
	}

	inline Vector2 operator*(const double b){
		Vector2 a = *this;
		a.X *= (float)b;
		a.Y *= (float)b;
		return a;
	}

	inline Vector2 operator*=(const float b){
		Vector2 a = *this;
		a.X *= b;
		a.Y *= b;
		return a;
	}

	inline Vector2 operator/(const int b) {
		Vector2 a = *this;
		a.X /= b;
		a.Y /= b;
		return a;
	}

	float dot(Vector2& b) {
		Vector2 a = *this;
	}
};



struct Rect {
	float X;
	float Y;
	float W;
	float H;

	/*operator iRect() const {
		return iRect{ (int)X, (int)Y, (int)W, (int)H };
	}*/

};

struct iRect {
	int X;
	int Y;
	int W;
	int H;

	operator Rect() const {

		return Rect{ 
			static_cast<float>(X), 
			static_cast<float>(Y), 
			static_cast<float>(W), 
			static_cast<float>(H) 
		};
	}

};
