#define DIRECTINPUT_VERSION 0x0700

#include "col_local.h"

extern CollisionPacket collisionPackage; // Stores all the parameters and returnvalues
extern int collisionRecursionDepth;      // Internal variable tracking the recursion depth

float **triangle_pool; // Stores the pointers to the traingles used for collision detection
int numtriangle;       // Number of traingles in pool
float unitsPerMeter;   // Set this to match application scale..
VECTOR gravity;        // Gravity

void checkTriangle(CollisionPacket *colPackage, VECTOR p1, VECTOR p2, VECTOR p3);

D3DVECTOR calculatemisslelength(D3DVECTOR velocity);

CollisionTrace trace; // Output structure telling the application everything about the collision

void checkCollision() {
	VECTOR P1, P2, P3;    // Temporary variables holding the triangle in R3
	VECTOR eP1, eP2, eP3; // Temporary variables holding the triangle in eSpace

	for (int i = 0; i < numtriangle; i++) // Iterate trough the entire triangle pool
	{
		// I'm sorry for my hard coding, but fill the traingle with the data from the pool
		P1.set(*triangle_pool[i * 9], *triangle_pool[i * 9 + 1], *triangle_pool[i * 9 + 2]);     // First vertex
		P2.set(*triangle_pool[i * 9 + 3], *triangle_pool[i * 9 + 4], *triangle_pool[i * 9 + 5]); // Second vertex
		P3.set(*triangle_pool[i * 9 + 6], *triangle_pool[i * 9 + 7], *triangle_pool[i * 9 + 8]); // Third vertex

		// Transform to eSpace
		eP1 = P1 / collisionPackage.eRadius;
		eP2 = P2 / collisionPackage.eRadius;
		eP3 = P3 / collisionPackage.eRadius;

		checkTriangle(&collisionPackage, eP1, eP2, eP3);
	}
}

VECTOR collideWithWorld(VECTOR pos, VECTOR vel) {
	// All hard-coded distances in this function is
	// scaled to fit the setting above..
	float unitScale = unitsPerMeter / 100.0f;
	float veryCloseDistance = 0.005f * unitScale;

	// do we need to worry?
	if (collisionRecursionDepth > 5)
		return pos;

	// Ok, we need to worry:
	collisionPackage.velocity = vel;
	collisionPackage.normalizedVelocity = vel;
	collisionPackage.normalizedVelocity.normalize();
	collisionPackage.velocityLength = vel.length();
	collisionPackage.basePoint = pos;
	collisionPackage.foundCollision = false;
	collisionPackage.nearestDistance = 10000000;

	// Check for collision (calls the collision routines)
	// Application specific!!
	checkCollision();

	// If no collision we just move along the velocity
	if (collisionPackage.foundCollision == false) {
		return pos + vel;
	}

	// *** Collision occured ***
	// The original destination point
	VECTOR destinationPoint = pos + vel;
	VECTOR newSourcePoint = pos;

	// only update if we are not already very close
	// and if so we only move very close to intersection..not
	// to the exact spot.
	if (collisionPackage.nearestDistance >= veryCloseDistance) {
		VECTOR V = vel;
		V.SetLength(collisionPackage.nearestDistance - veryCloseDistance);
		//		newSourcePoint = collisionPackage.sourcePoint + V;
		// fixed by silencerex
		newSourcePoint = collisionPackage.basePoint + V;

		// Adjust polygon intersection point (so sliding
		// plane will be unaffected by the fact that we
		// move slightly less than collision tells us)
		V.normalize();
		collisionPackage.intersectionPoint = collisionPackage.intersectionPoint - V * veryCloseDistance;
	}

	// Determine the sliding plane
	VECTOR slidePlaneOrigin = collisionPackage.intersectionPoint;
	VECTOR slidePlaneNormal = newSourcePoint - collisionPackage.intersectionPoint;
	// fixed by tele forgot to normalize slideplane
	slidePlaneNormal.normalize();

	PLANE slidingPlane(slidePlaneOrigin, slidePlaneNormal);

	// Again, sorry about formatting.. but look carefully ;)
	VECTOR newDestinationPoint = destinationPoint - slidePlaneNormal * slidingPlane.signedDistanceTo(destinationPoint);

	// Generate the slide vector, which will become our new
	// velocity vector for the next iteration
	VECTOR newVelocityVector = newDestinationPoint - collisionPackage.intersectionPoint;

	// Recurse:
	// dont recurse if the new velocity is very small
	if (newVelocityVector.length() < veryCloseDistance) {
		return newSourcePoint;
	}

	collisionRecursionDepth++;
	return collideWithWorld(newSourcePoint, newVelocityVector);
}

CollisionTrace *collideAndSlide(float *position, float *vel) {
	// Do collision detection:
	collisionPackage.R3Position.set(position[0], position[1], position[2]);
	collisionPackage.R3Velocity.set(vel[0], vel[1], vel[2]);

	// calculate position and velocity in eSpace
	VECTOR eSpacePosition = collisionPackage.R3Position / collisionPackage.eRadius;
	VECTOR eSpaceVelocity = collisionPackage.R3Velocity / collisionPackage.eRadius;

	// Iterate until we have our final position.
	collisionRecursionDepth = 0;
	VECTOR finalPosition = collideWithWorld(eSpacePosition, eSpaceVelocity);

	// Add gravity pull:
	// To remove gravity uncomment from here .....
	// Set the new R3 position (convert back from eSpace to R3
	collisionPackage.R3Position = finalPosition * collisionPackage.eRadius;
	collisionPackage.R3Velocity = gravity;
	eSpaceVelocity = gravity / collisionPackage.eRadius;
	collisionRecursionDepth = 0;
	finalPosition = collideWithWorld(finalPosition, eSpaceVelocity);

	// ... to here
	// Convert final result back to R3:
	finalPosition = finalPosition * collisionPackage.eRadius;
	collisionPackage.intersectionPoint = collisionPackage.intersectionPoint * collisionPackage.eRadius;

	// Output the results in the CollisionTrace structure
	finalPosition.get((float **)&trace.finalPosition);
	trace.foundCollision = collisionPackage.foundCollision;
	trace.nearestDistance = (collisionPackage.intersectionPoint - collisionPackage.R3Position).length();
	collisionPackage.intersectionPoint.get((float **)&trace.intersectionPoint);

	return &trace; // Return the adress of the Trace structure to the application
}
/*
extern "C" __declspec(dllexport) void colSetUnitsPerMeter(float UPM)
{
    unitsPerMeter=UPM;
}

extern "C" __declspec(dllexport) void colSetGravity(float* grav )
{
    gravity.set(grav[0], grav[1], grav[2]);
}

extern "C" __declspec(dllexport) void colSetRadius(float radius)
{
    collisionPackage.eRadius=radius;
}

extern "C" __declspec(dllexport) void colSetTrianglePool(int numtri, float** pool)
{
    numtriangle=numtri;
    triangle_pool=pool;
}
*/

D3DVECTOR calculatemisslelength(D3DVECTOR velocity) {

	D3DVECTOR result;
	VECTOR v;

	v.x = velocity.x;
	v.y = velocity.y;
	v.z = velocity.z;

	v.SetLength(1);

	result.x = v.x;
	result.y = v.y;
	result.z = v.z;
	return result;
}