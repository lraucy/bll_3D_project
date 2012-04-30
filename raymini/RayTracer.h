// *********************************************************
// Ray Tracer Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2012 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <iostream>
#include <vector>
#include <QImage>

#include "Vec3D.h"
#include "Ray.h"
#include "Object.h"
#include "Light.h"

class Scene;

class RayTracer {
public:
    static RayTracer * getInstance ();
    static void destroyInstance ();

    inline const Vec3Df & getBackgroundColor () const { return backgroundColor;}
    inline void setBackgroundColor (const Vec3Df & c) { backgroundColor = c; }
    
    QImage render (const Vec3Df & camPos,
                   const Vec3Df & viewDirection,
                   const Vec3Df & upVector,
                   const Vec3Df & rightVector,
                   float fieldOfView,
                   float aspectRatio,
                   unsigned int screenWidth,
                   unsigned int screenHeight);
    
protected:
    inline RayTracer () {}
    inline virtual ~RayTracer () {}
	const Object * getObjectIntersected(const Scene * scene, const Vec3Df &camPos,
									const Vec3Df &dir, Vec3Df &intersectionPoint,
									Triangle &intersectionTriangle) const;

	Vec3Df getNormalAtIntersection(const Object &o, const Vec3Df &intersectionPoint,
										const Triangle &triangle) const;

	Vec3Df getPhongBRDF(const Scene * scene, const Ray &ray, const Object &o,
						const Vec3Df &intersectionPoint, const Vec3Df &normal) const;

  Vec3Df getPhongBRDFwithLights(const Scene * scene,const std::vector<Light>lights, const Ray &ray, const Object &o,
						const Vec3Df &intersectionPoint, const Vec3Df &normal) const;

  Vec3Df pathTracer(const Scene * scene, const Ray & ray, const Object & intersectedObject, Vertex & intP, unsigned int depth) const;
  Vec3Df directionFromHemisphere(const Vec3Df &intersectionPoint, const Vec3Df &normal)const;
  std::vector<Vec3Df> getPathTracingDirection(Vertex & v)const;
  Vec3Df ambientOcclusion(Scene * scene, const Ray & ray, const Object & intersectedObject, Vertex & intP)const;
private:
    Vec3Df backgroundColor;
};


#endif // RAYTRACER_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
