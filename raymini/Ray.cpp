// *********************************************************
// Ray Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Ray.h"
#include "Mesh.h"
#include "Triangle.h"
#define SMALL_NUM  0.00000001 
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


bool Ray::intersectWithTriangle(const Mesh & mesh, const Triangle & triangle, Vec3Df & intersectionPoint )const{
  /*http://softsurfer.com/Archive/algorithm_0105/algorithm_0105.htm#intersect_RayTriangle()*/

  Vec3Df u,v,n;   //triangle vectors
  Vec3Df w0, w;   //ray vector
  float r,a,b;    //params to calcul ray-plane intersect

  /* get triangle edge vectors and plane normal*/
  u = mesh.getVertices()[triangle.getVertex(1)].getPos() - mesh.getVertices()[triangle.getVertex(0)].getPos();
  v = mesh.getVertices()[triangle.getVertex(2)].getPos() - mesh.getVertices()[triangle.getVertex(0)].getPos();
  n = Vec3Df::crossProduct(u, v);

  /* triangle doesnt exist */
  // if (n.p[0] == 0)
  //  return (false);
  
  w0 = origin - mesh.getVertices()[triangle.getVertex(0)].getPos();
  a = -Vec3Df::dotProduct(n, w0);
  b = Vec3Df::dotProduct(n, direction);

  if (fabs(b)<SMALL_NUM){ //ray is parallel to triangle plane
    if(a==0)              //ray lies in triangle plane
      return false;
    else return false;    //ray disjoint from plane
  }

  /* get intersect point of ray with triangle plane */
  r = a / b;

  if(r < 0.0)             //ray goes away from  triangle
    return false;
  //for a segment need to test if(r>1.0) no intersect

  intersectionPoint = origin + r*direction;
  
  float uu, uv, vv, wu, wv, D;
  uu = Vec3Df::dotProduct(u, u);
  uv = Vec3Df::dotProduct(u, v);
  vv = Vec3Df::dotProduct(v, v);
  w =  intersectionPoint - mesh.getVertices()[triangle.getVertex(0)].getPos();
  wu = Vec3Df::dotProduct(w, u);
  wv = Vec3Df::dotProduct(w, v);
  D = uv * uv - uu * vv;
  float s, t;

  s = (uv * wv - vv * wu) / D;

  if (s<0.0 || s>1.0)
    return false;

  t= (uv * wu - uu * wv) / D;

  if( t <0.0 || (s+t) > 1.0)
    return false;
return (true);
}
