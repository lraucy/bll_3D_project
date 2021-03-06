// *********************************************************
// Ray Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef RAY_H
#define RAY_H

#include <iostream>
#include <vector>

#include "Vec3D.h"
#include "Mesh.h"
#include "Triangle.h"
#include "BoundingBox.h"

class Ray {
public:
    inline Ray () {isASegment = false; intersectReverseTriangles = false;}
    inline Ray (const Vec3Df & origin, const Vec3Df & direction)
        : origin (origin), direction (direction) { isASegment = false; intersectReverseTriangles = false; }
    inline virtual ~Ray () {}

    inline const Vec3Df & getOrigin () const { return origin; }
    inline Vec3Df & getOrigin () { return origin; }
    inline const Vec3Df & getDirection () const { return direction; }
    inline Vec3Df & getDirection () { return direction; }

	inline void translate(const Vec3Df &trans) { origin += trans; }

	static Ray * getRandomRay(const Vec3Df &origin, const Vec3Df &normal,
			const Vec3Df &secondVec, const Vec3Df &thirdVec, float coneAngle);

    bool intersect (const BoundingBox & bbox, Vec3Df & intersectionPoint) const;
    bool intersect (const Mesh & mesh, const Triangle & triangle, Vec3Df & intersectionPoint) const;
    bool intersect (const Mesh & mesh, const Triangle & triangle, Vec3Df & intersectionPoint, bool &reverse) const;
	bool intersect (const Mesh & mesh, KdTreeElement *kdTree, Vec3Df & intersectionPoint, float &intersectionDistance, Triangle &triangle);
	bool intersect (const Mesh & mesh, KdTreeElement *kdTree) const;
	bool intersectInSphere (const Mesh &mesh, KdTreeElement *kdTree,
		const Vec3Df &originSphere, float R) const;

	bool isASegment;
	bool intersectReverseTriangles;
	bool reverseTriangleIntersected;
    
private:
    Vec3Df origin;
    Vec3Df direction;
};


#endif // RAY_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
