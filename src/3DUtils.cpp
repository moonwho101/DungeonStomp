// ---------------------------------------------------------------------------------------------------------------------------------
//  ____  _____  _    _ _   _ _                          
// |___ \|  __ \| |  | | | (_) |                         
//   __) | |  | | |  | | |_ _| |___      ___ _ __  _ __  
//  |__ <| |  | | |  | | __| | / __|    / __| '_ \| '_ \ 
//  ___) | |__| | |__| | |_| | \__ \ _ | (__| |_) | |_) |
// |____/|_____/ \____/ \__|_|_|___/(_) \___| .__/| .__/ 
//                                          | |   | |    
//                                          |_|   |_|    
//
// Utilitarian funcitons useful for 3D graphics
//
// Best viewed with 8-character tabs and (at least) 132 columns
//
// ---------------------------------------------------------------------------------------------------------------------------------
//
// Restrictions & freedoms pertaining to usage and redistribution of this software:
//
//  * This software is 100% free
//  * If you use this software (in part or in whole) you must credit the author.
//  * This software may not be re-distributed (in part or in whole) in a modified
//    form without clear documentation on how to obtain a copy of the original work.
//  * You may not use this software to directly or indirectly cause harm to others.
//  * This software is provided as-is and without warrantee. Use at your own risk.
//
// For more information, visit HTTP://www.FluidStudios.com
//
// ---------------------------------------------------------------------------------------------------------------------------------
// Originally created on 12/20/2000 by Paul Nettle
//
// Copyright 2000, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
#include "prag.h"
#include "stdafx.h"
#include "Common.h"
#include "VMath.h"
#include "3DUtils.h"

// ---------------------------------------------------------------------------------------------------------------------------------
// Returns a random number between 0...1
// ---------------------------------------------------------------------------------------------------------------------------------

FLT	frand()
{
	return (FLT)rand() / (FLT)RAND_MAX;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Returns the closest point on the line-segment, and sets the 'edge' flag if the point is not one of the endpoints
// ---------------------------------------------------------------------------------------------------------------------------------

point3	closestPointOnLine(const point3& a, const point3& b, const point3& p, bool& edge)
{
	vector3 c = p - a;
	vector3 v = b - a;
	v.normalize();

	edge = false;
	FLT	t = v ^ c;
	if (t < 0) return a;

	FLT	d = b.distance(a);
	if (t > d) return b;

	edge = true;
	return a + v * t;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// 3DUtils.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------



