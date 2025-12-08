/*
 * Types.h
 * Copyright (C) 1999 by Tobias M. Lensing
 *
 * This file is a modified version of the Cyclon3D type base.
 *
 */

#ifndef _TYPES_H
#define _TYPES_H

#define D3D_OVERLOADS
#include "D3DMath.h"
#include "D3DUtil.h"
#include "TypeBase.h"
#include "LineBase.h"

//////////////////////////////////////////////////////////////////////
// Basic mathematical macros
#define sqr(x) ((x) * (x))
#define dbl(x) ((x) * 2)
#define reciprocal(x) (1 / (x))
#define rad(x) ((x) * (g_PI) / 180)
#define deg(x) ((x) / (g_PI) * 180)

//////////////////////////////////////////////////////////////////////
// Helper macros
#define WallMeshVertices(x) (sqr(x))          // Obsolete
#define WallMeshIndices(x) (sqr((x) - 1) * 6) // Obsolete

//////////////////////////////////////////////////////////////////////
// Type Definitions
typedef float FPOINT;
typedef double FDOUBLE;

//////////////////////////////////////////////////////////////////////
// Constant definitions

// #define INFINITY			1e+10
#define OFF 0
#define ON 1

#define DIR_LEFT 1
#define DIR_RIGHT 2
#define DIR_UP 3
#define DIR_DOWN 4
#define DIR_FORWARD 5
#define DIR_BACKWARD 6

#define PLANE_XY 1
#define PLANE_XZ 2
#define PLANE_YZ 3

class Vector2D : public CTypeBase {
  public:
	Vector2D() {
		Vector2D(0, 0);
	}
	Vector2D(FPOINT _x, FPOINT _y) {
		x = _x;
		y = _y;
	}

	FPOINT x, y;
};

class Vector3D : public CTypeBase // compatible to D3DVECTOR!
{
  public:
	Vector3D() {
		Vector3D(0, 0, 0);
	}
	Vector3D(FPOINT _x, FPOINT _y, FPOINT _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	D3DVECTOR GetD3DVector() {
		return D3DVECTOR(x, y, z);
	}

	friend Vector3D operator+(Vector3D &v0, Vector3D &v1);
	friend Vector3D operator-(Vector3D &v0, Vector3D &v1);
	friend Vector3D operator*(Vector3D &v0, Vector3D &v1);
	friend Vector3D operator/(Vector3D &v0, Vector3D &v1);
	friend Vector3D operator+=(Vector3D &v0, Vector3D &v1);

	FPOINT x, y, z;
};

#define Point2D Vector2D
#define Point3D Vector3D

// The plane object represents a simple infinite plane in the 3D world
class Plane : public CTypeBase {
  public:
	Plane() {
		Plane(0, 0, 0, 0);
	}
	Plane(FPOINT _a, FPOINT _b, FPOINT _c, FPOINT _d) {
		vNormal = Vector3D(_a, _b, _c);
		d = _d;
	}
	Plane(Vector3D _vNormal, FPOINT _d) {
		vNormal = _vNormal;
		d = _d;
	}

	// Calculates the distance of the plane using a point which lies
	// on the plane
	void CalcDist(Vector3D *pvPoint);

	// Gets the result of the plane equation
	FPOINT ComputePoint(Vector3D *pvPoint);

	Vector3D vNormal;
	FPOINT d;
};

// 3D ray with origin and direction, but without end point - infinite on the
// direction vector side
class Ray3D : public CLineBase {
  public:
	Ray3D() {
		Ray3D(Vector3D(0, 0, 0), Vector3D(0, 0, 0));
		nLineType = LINETYPE_RAY3D;
	}
	Ray3D(Vector3D _vOrigin, Vector3D _vDirection) {
		vOrigin = _vOrigin;
		vDirection = _vDirection;
		nLineType = LINETYPE_RAY3D;
	}

	Vector3D vOrigin;
	Vector3D vDirection;
};

// 3D line with start and end point, but completely infinite
class Line3D : public CLineBase {
  public:
	Line3D() {
		Line3D(Vector3D(0, 0, 0), Vector3D(0, 0, 0));
		nLineType = LINETYPE_LINE3D;
	}
	Line3D(Vector3D _v0, Vector3D _v1) {
		v0 = _v0;
		v1 = _v1;
		nLineType = LINETYPE_LINE3D;
	}

	Vector3D v0, v1;
};

// 3D segment which is not infinite at all - with start and end point
class Segment3D : public CLineBase {
  public:
	Segment3D() {
		Segment3D(Vector3D(0, 0, 0), Vector3D(0, 0, 0));
		nLineType = LINETYPE_SEGMENT3D;
	}
	Segment3D(Vector3D _v0, Vector3D _v1) {
		v0 = _v0;
		v1 = _v1;
		nLineType = LINETYPE_SEGMENT3D;
	}

	Vector3D v0, v1;
};

// 2-Dimensional polygon (transformed)
class Polygon2 {
  public:
	Polygon2() {
		Polygon2(Vector2D(0, 0), Vector2D(0, 0), Vector2D(0, 0));
	}
	Polygon2(Vector2D _v0, Vector2D _v1, Vector2D _v2) {
		v0 = _v0;
		v1 = _v1;
		v2 = _v2;
	}

	Vector2D v0, v1, v2;
};

// 3-Dimensional polygon
class Polygon3 {
  public:
	Polygon3() {
		Polygon3(Vector3D(0, 0, 0), Vector3D(0, 0, 0), Vector3D(0, 0, 0));
	}
	Polygon3(Vector3D _v0, Vector3D _v1, Vector3D _v2) {
		v0 = _v0;
		v1 = _v1;
		v2 = _v2;
	}

	Vector3D v0, v1, v2;
};

class TRGBA {
  public:
	TRGBA() {
	}
	TRGBA(float _r, float _g, float _b, float _a) {
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}
	TRGBA(DWORD dwColor) {
		SplitColors(dwColor);
	}
	void SplitColors(DWORD dwColor) {
		r = (float)RGBA_GETRED(dwColor) / 255;
		g = (float)RGBA_GETGREEN(dwColor) / 255;
		b = (float)RGBA_GETBLUE(dwColor) / 255;
		a = (float)RGBA_GETALPHA(dwColor) / 255;
	}
	bool equals(TRGBA t) {
		if (t.r == r && t.g == g && t.b == b && t.a == a)
			return true;
		else
			return false;
	}
	bool IsSimilarTo(TRGBA t) {
		if (t.r >= r - 0.1 && t.r <= r + 0.1 &&
		    t.g >= g - 0.1 && t.g <= g + 0.1 &&
		    t.b >= b - 0.1 && t.b <= b + 0.1 &&
		    t.a >= a - 0.1 && t.a <= a + 0.1)
			return true;
		else
			return false;
	}
	D3DCOLOR GetColor() {
		return D3DRGBA(r, g, b, a);
	}
	float r, g, b, a;
};

#endif // _TYPES_H