#define DIRECTINPUT_VERSION 0x0700
//-----------------------------------------------------------------------------
// File: Vectormath.cpp
//
// Desc: A collection of vector math related functions.
//
// Copyright (c) 2000 Telemachos of Peroxide
// www.peroxide.dk
//-----------------------------------------------------------------------------

#define D3D_OVERLOADS
#define STRICT
#include "prag.h"
#include <math.h>
#include <stdio.h>
#include "vectormath.h"

// ----------------------------------------------------------------------
// Name  : intersectRayPlane()
// Input : rOrigin - origin of ray in world space
//         rVector - vector describing direction of ray in world space
//         pOrigin - Origin of plane
//         pNormal - Normal to plane
// Notes : Normalized directional vectors expected
// Return: distance to plane in world units, -1 if no intersection.
// -----------------------------------------------------------------------
double intersectRayPlane(D3DVECTOR rOrigin, D3DVECTOR rVector, D3DVECTOR pOrigin, D3DVECTOR pNormal) {

	double d = -(dot(pNormal, pOrigin));

	double numer = dot(pNormal, rOrigin) + d;
	double denom = dot(pNormal, rVector);

	if (denom == 0) // normal is orthogonal to vector, cant intersect
		return (-1.0f);

	return -(numer / denom);
}

// ----------------------------------------------------------------------
// Name  : intersectRaySphere()
// Input : rO - origin of ray in world space
//         rV - vector describing direction of ray in world space
//         sO - Origin of sphere
//         sR - radius of sphere
// Notes : Normalized directional vectors expected
// Return: distance to sphere in world units, -1 if no intersection.
// -----------------------------------------------------------------------

double intersectRaySphere(D3DVECTOR rO, D3DVECTOR rV, D3DVECTOR sO, double sR) {

	D3DVECTOR Q = sO - rO;

	double c = lengthOfVector(Q);
	double v = dot(Q, rV);
	double d = sR * sR - (c * c - v * v);

	// If there was no intersection, return -1
	if (d < 0.0)
		return (-1.0f);

	// Return the distance to the [first] intersecting point
	return (v - sqrt(d));
}

// ----------------------------------------------------------------------
// Name  : CheckPointInTriangle()
// Input : point - point we wish to check for inclusion
//         a - first vertex in triangle
//         b - second vertex in triangle
//         c - third vertex in triangle
// Notes : Triangle should be defined in clockwise order a,b,c
// Return: TRUE if point is in triangle, FALSE if not.
// -----------------------------------------------------------------------

BOOL CheckPointInTriangle(D3DVECTOR point, D3DVECTOR a, D3DVECTOR b, D3DVECTOR c) {

	double total_angles = 0.0f;
	double PI = 3.14159265358979323846f;

	// make the 3 vectors
	D3DVECTOR v1 = point - a;
	D3DVECTOR v2 = point - b;
	D3DVECTOR v3 = point - c;

	normalizeVector(v1);
	normalizeVector(v2);
	normalizeVector(v3);

	total_angles += acos(dot(v1, v2));
	total_angles += acos(dot(v2, v3));
	total_angles += acos(dot(v3, v1));

	if (fabs(total_angles - 2 * PI) <= 0.005)
		return (TRUE);

	return (FALSE);
}

/*


BOOL CheckPointInTriangle(const VECTOR& point, const VECTOR& a, const VECTOR& b, const VECTOR& c) {
//new one
int pos = 0;
int neg = 0;

float halfplane;
// Calculate normal to plane of triangle
VECTOR v1 = b-a; // edge a->b

VECTOR v2 = c-a;
VECTOR normal = v1^v2;
normal.normalize();

// Create a halfplane for each edge : a->b , b->c and c->a

// a->b
VECTOR n = v1^normal;
halfplane = point%n - a%n;

if (halfplane > EPSILON)
pos++;
else if(halfplane < -EPSILON)
neg++;

// b->c
v1 = c-b;
n = v1^normal;
halfplane = point%n - b%n;

if (halfplane > EPSILON)
pos++;
else if(halfplane < -EPSILON)
neg++;

// c->a
v1 = a-c;
n = v1^normal;
halfplane = point%n - c%n;

if (halfplane > EPSILON)
pos++;
else if(halfplane < -EPSILON)
neg++;

// result:
if (!neg || !pos)
return true;

return false;
}

*/

// ----------------------------------------------------------------------
// Name  : closestPointOnLine()
// Input : a - first end of line segment
//         b - second end of line segment
//         p - point we wish to find closest point on line from
// Notes : Helper function for closestPointOnTriangle()
// Return: closest point on line segment
// -----------------------------------------------------------------------

inline D3DVECTOR closestPointOnLine(D3DVECTOR &a, D3DVECTOR &b, D3DVECTOR &p) {

	// Determine t (the length of the vector from ‘a’ to ‘p’)
	D3DVECTOR c = p - a;
	D3DVECTOR V = b - a;

	double d = lengthOfVector(V);

	normalizeVector(V);
	double t = dot(V, c);

	// Check to see if ‘t’ is beyond the extents of the line segment
	if (t < 0.0f)
		return (a);
	if (t > d)
		return (b);

	// Return the point between ‘a’ and ‘b’
	// set length of V to t. V is normalized so this is easy
	V.x = V.x * (float)t;
	V.y = V.y * (float)t;
	V.z = V.z * (float)t;

	return (a + V);
}

// ----------------------------------------------------------------------
// Name  : closestPointOnTriangle()
// Input : a - first vertex in triangle
//         b - second vertex in triangle
//         c - third vertex in triangle
//         p - point we wish to find closest point on triangle from
// Notes :
// Return: closest point on line triangle edge
// -----------------------------------------------------------------------

D3DVECTOR closestPointOnTriangle(D3DVECTOR a, D3DVECTOR b, D3DVECTOR c, D3DVECTOR p) {

	D3DVECTOR Rab = closestPointOnLine(a, b, p);
	D3DVECTOR Rbc = closestPointOnLine(b, c, p);
	D3DVECTOR Rca = closestPointOnLine(c, a, p);

	double dAB = lengthOfVector(p - Rab);
	double dBC = lengthOfVector(p - Rbc);
	double dCA = lengthOfVector(p - Rca);

	double min = dAB;
	D3DVECTOR result = Rab;

	if (dBC < min) {
		min = dBC;
		result = Rbc;
	}

	if (dCA < min)
		result = Rca;

	return (result);
}

// ----------------------------------------------------------------------
// Name  : CheckPointInTriangle()
// Input : point - point we wish to check for inclusion
//         sO - Origin of sphere
//         sR - radius of sphere
// Notes :
// Return: TRUE if point is in sphere, FALSE if not.
// -----------------------------------------------------------------------

BOOL CheckPointInSphere(D3DVECTOR point, D3DVECTOR sO, double sR) {

	float d = (float)lengthOfVector(point - sO);

	if (d <= sR)
		return TRUE;
	return FALSE;
}

// ----------------------------------------------------------------------
// Name  : tangentPlaneNormalOfEllipsoid()
// Input : point - point we wish to compute normal at
//         eO - Origin of ellipsoid
//         eR - radius vector of ellipsoid
// Notes :
// Return: a unit normal vector to the tangent plane of the ellipsoid in the point.
// -----------------------------------------------------------------------
D3DVECTOR tangentPlaneNormalOfEllipsoid(D3DVECTOR point, D3DVECTOR eO, D3DVECTOR eR) {

	D3DVECTOR p = point - eO;

	double a2 = eR.x * eR.x;
	double b2 = eR.y * eR.y;
	double c2 = eR.z * eR.z;

	D3DVECTOR res;
	res.x = p.x / (float)a2;
	res.y = p.y / (float)b2;
	res.z = p.z / (float)c2;

	normalizeVector(res);
	return (res);
}

// ----------------------------------------------------------------------
// Name  : classifyPoint()
// Input : point - point we wish to classify
//         pO - Origin of plane
//         pN - Normal to plane
// Notes :
// Return: One of 3 classification codes
// -----------------------------------------------------------------------

DWORD classifyPoint(D3DVECTOR point, D3DVECTOR pO, D3DVECTOR pN) {

	D3DVECTOR dir = pO - point;
	double d = dot(dir, pN);

	if (d < -0.001f)
		return PLANE_FRONT2;
	else if (d > 0.001f)
		return PLANE_BACKSIDE;

	return ON_PLANE;
}
