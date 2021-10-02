// ---------------------------------------------------------------------------------------------------------------------------------
//  _     _           _                 _     
// | |   | |         | |               | |    
// | |   | | ___ _ __| |_  _____  __   | |__  
//  \ \ / / / _ \ '__| __|/ _ \ \/ /   | '_ \ 
//   \ V / |  __/ |  | |_|  __/>  <  _ | | | |
//    \_/   \___|_|   \__|\___/_/\_\(_)|_| |_|
//                                            
//                                            
//
// Simplistic vertex management class
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
// Originally created on 12/06/2000 by Paul Nettle
//
// Copyright 2000, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef	_H_VERTEX
#define	_H_VERTEX
#include "prag.h"

// ---------------------------------------------------------------------------------------------------------------------------------

template<class T = FLT>
class	vert
{
public:
	// Construction/Destruction

inline				vert() {}
inline				vert(const vert &v) : _texture(v.texture()), _world(v.world()), _screen(v.screen()) {}
inline				vert(const point3 &p)	: _world(p) {}

	// Operators

inline		vert		&operator =(const vert &v)
				{
					texture() = v.texture();
					world() = v.world();
					screen() = v.screen();
					return *this;
				}

	// Cast world-space vertex to a point

inline				operator point2() const	{return texture();}
inline				operator point2()	{return texture();}
inline				operator point3() const	{return world();}
inline				operator point3()	{return world();}
inline				operator point4() const	{return screen();}
inline				operator point4()	{return screen();}

	// Accessors

inline	const	point2		&texture() const{return _texture;}
inline		point2		&texture()	{return _texture;}
inline	const	point3		&world() const	{return _world;}
inline		point3		&world()	{return _world;}
inline	const	point4		&screen() const	{return _screen;}
inline		point4		&screen()	{return _screen;}

protected:
	// Data

		matrix<2, 1, T>	_texture;
		matrix<3, 1, T>	_world;
		matrix<4, 1, T>	_screen;
};

#endif // _H_VERTEX
// ---------------------------------------------------------------------------------------------------------------------------------
// Vertex.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

