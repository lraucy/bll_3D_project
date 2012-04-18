// *********************************************************
// Ray Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Ray.h"

using namespace std;

static const unsigned int NUMDIM = 3, RIGHT = 0, LEFT = 1, MIDDLE = 2;

bool Ray::intersect (const BoundingBox & bbox, Vec3Df & intersectionPoint) const {
    const Vec3Df & minBb = bbox.getMin ();
    const Vec3Df & maxBb = bbox.getMax ();
    bool inside = true;
    unsigned int  quadrant[NUMDIM];
    register unsigned int i;
    unsigned int whichPlane;
    Vec3Df maxT;
    Vec3Df candidatePlane;
    
    for (i=0; i<NUMDIM; i++)
        if (origin[i] < minBb[i]) {
            quadrant[i] = LEFT;
            candidatePlane[i] = minBb[i];
            inside = false;
        } else if (origin[i] > maxBb[i]) {
            quadrant[i] = RIGHT;
            candidatePlane[i] = maxBb[i];
            inside = false;
        } else	{
            quadrant[i] = MIDDLE;
        }

    if (inside)	{
        intersectionPoint = origin;
        return (true);
    }

    for (i = 0; i < NUMDIM; i++)
        if (quadrant[i] != MIDDLE && direction[i] !=0.)
            maxT[i] = (candidatePlane[i]-origin[i]) / direction[i];
        else
            maxT[i] = -1.;

    whichPlane = 0;
    for (i = 1; i < NUMDIM; i++)
        if (maxT[whichPlane] < maxT[i])
            whichPlane = i;

    if (maxT[whichPlane] < 0.) return (false);
    for (i = 0; i < NUMDIM; i++)
        if (whichPlane != i) {
            intersectionPoint[i] = origin[i] + maxT[whichPlane] *direction[i];
            if (intersectionPoint[i] < minBb[i] || intersectionPoint[i] > maxBb[i])
                return (false);
        } else {
            intersectionPoint[i] = candidatePlane[i];
        }
    return (true);			
}

#define SMALL_NUMBER 0.0000001
bool Ray::intersect (const Mesh & mesh, const Triangle & triangle, Vec3Df & intersectionPoint) const {
	// method found on http://softsurfer.com/Archive/algorithm_0105/algorithm_0105.htm
	const std::vector<Vertex> & vertices = mesh.getVertices();
	const Vec3Df & v0 = vertices[triangle.getVertex(0)].getPos();
	const Vec3Df & v1 = vertices[triangle.getVertex(1)].getPos();
	const Vec3Df & v2 = vertices[triangle.getVertex(2)].getPos();

	Vec3Df u = v1 - v0;
	Vec3Df v = v2 - v0;
	Vec3Df n = Vec3Df::crossProduct(u,v);
	if(n == Vec3Df(0.0,0.0,0.0))
		return false;

	float denominator = (Vec3Df::dotProduct(n, this->direction));
	if (denominator > 0 // check face orientation
			|| - denominator < SMALL_NUMBER) // check parallelism between ray and triangle
		return false;
	float r = Vec3Df::dotProduct(n,v0 - this->origin) / denominator;
	if (r < 0.0)
		return false;
	intersectionPoint = this->origin + r * this->direction;

	Vec3Df w = intersectionPoint - v0;

	float dotuu = Vec3Df::dotProduct(u,u);
	float dotvv = Vec3Df::dotProduct(v,v);
	float dotuv = Vec3Df::dotProduct(u,v);
	float dotuw = Vec3Df::dotProduct(u,w);
	float dotvw = Vec3Df::dotProduct(v,w);
	denominator = dotuv*dotuv - dotuu * dotvv;

	float s = (dotuv*dotvw - dotvv*dotuw)/ denominator;
	if(s < 0.0 || s > 1.0)
		return false;

	float t = (dotuv*dotuw - dotuu*dotvw) / denominator;
	if (t < 0.0 || s+t > 1.0)
		return false;
	else
		return true;
}
