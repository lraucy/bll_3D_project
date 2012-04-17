// *********************************************************
// Ray Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Ray.h"

using namespace std;

static const unsigned int NUMDIM = 3, RIGHT = 0, LEFT = 1, MIDDLE = 2;

float air(const Vec3Df t[3]){
    float det1, det2, det3;
    det1 = t[0][0] * (t[1][1] - t[2][1]) - t[0][1] * (t[1][0] - t[2][0]) + (t[1][0]*t[2][1] - t[2][0]*t[1][1]);
    det2 = t[0][1] * (t[1][2] - t[2][2]) - t[0][2] * (t[1][1] - t[2][1]) + (t[1][1]*t[2][2] - t[2][1]*t[1][2]);
    det3 = t[0][2] * (t[1][0] - t[2][0]) - t[0][0] * (t[1][2] - t[2][2]) + (t[1][2]*t[2][0] - t[2][2]*t[1][0]);
    return (sqrt(det1*det1 + det2*det2 + det3*det3)/2);
}

bool Ray::intersectTriangle(const Vec3Df triangle[3], Vec3Df & intersectionPoint, Vec3Df & barycentricCoo) {
    Vec3Df    u, v, n;        
    Vec3Df    w0, w;          
    float     r, a, b;            
    
    u = triangle[1] - triangle[0];
    v = triangle[2] - triangle[0];
    n = Vec3Df::crossProduct(u, v);        
 
    w0 = origin - triangle[0];
    a = -Vec3Df::dotProduct(n,w0);
    b = Vec3Df::dotProduct(n,direction);

    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < 0.0)                   // ray goes away from triangle
        return false;                  // => no intersect

    intersectionPoint = origin + r * direction;           // intersect point of ray and plane

    // is I inside T?
    float    uu, uv, vv, wu, wv, D;
    uu = Vec3Df::dotProduct(u,u);
    uv = Vec3Df::dotProduct(u,v);
    vv = Vec3Df::dotProduct(v,v);
    w =  intersectionPoint - triangle[0];
    wu = Vec3Df::dotProduct(w,u);
    wv = Vec3Df::dotProduct(w,v);
    D = uv * uv - uu * vv;

    // get and test parametric coords
    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0)        // I is outside T
        return false;
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0)  // I is outside T
        return false;

    // calculate barycentric coordinates
    Vec3Df PA, PB, PC;
    PA = triangle[0] - intersectionPoint;
    PB = triangle[1] - intersectionPoint;
    PC = triangle[2] - intersectionPoint;
    
    
    float t_air = air(triangle);
    Vec3Df t_1[3] = {intersectionPoint, triangle[1], triangle[2]};
    float alpha = air(t_1) / t_air;
    Vec3Df t_2[3] = {triangle[0], intersectionPoint, triangle[2]};
    float beta = air(t_2) / t_air;
    Vec3Df t_3[3] = {triangle[0], triangle[1], intersectionPoint};
    float gama = air(t_3) / t_air;


    barycentricCoo = Vec3Df(alpha, beta, gama);
      
    return true;                    
 }
   
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
