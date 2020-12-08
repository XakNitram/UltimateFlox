#pragma once
#include "../pch.hpp"

struct Vector;

struct Point {
	float x, y;

	Point(float x, float y);
	Point();

	Point& operator+=(const Point&);
	Point& operator+=(const Vector&);
	Point& operator+=(const float);

	const Point operator+(const Point& rhs);
	const Point operator+(const Vector& rhs);
	const Point operator+(const float);

	float distanceTo(const Point& other) const;
};

struct Vector {
	float x, y;

	Vector();
	Vector(float x, float y);
	Vector(const Vector&);

	// Addition
	Vector& operator+=(const Vector& rhs);
	Vector& operator+=(const float rhs);
	const Vector operator+(const Vector& rhs);
	const Vector operator+(const float rhs);

	// Multiplication
	Vector& operator*=(const Vector& rhs);
	Vector& operator*=(const float rhs);
	const Vector operator*(const Vector& rhs);
	const Vector operator*(const float rhs);

	float angle() const;
};

struct LineSegment {
	Point a, b;

	LineSegment(float x1, float y1, float x2, float y2);
	LineSegment(Point a, Point b);
};

struct Ray {
	Point pos;
	Vector dir;
	
	Ray(float x, float y, float angle);
	Ray();

	std::optional<Point> intersects(const LineSegment& bound) const;
};
