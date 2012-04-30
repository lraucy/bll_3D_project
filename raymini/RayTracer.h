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

#define RAYTRACER_NO_SHADOW 0
#define RAYTRACER_HARD_SHADOW 1
#define RAYTRACER_SOFT_SHADOW 2

#define RAYTRACER_NO_AA 0
#define RAYTRACER_AAx2 1
#define RAYTRACER_AAx3 2



class Scene;

class RayTracer {
public:
    static RayTracer * getInstance ();
    static void destroyInstance ();

    inline const Vec3Df & getBackgroundColor () const { return backgroundColor;}
    inline void setBackgroundColor (const Vec3Df & c) { backgroundColor = c; }
    
  inline void setAOOption(const bool _aoOpt) { aoOpt = _aoOpt;}
  inline void setShadowOption(const int _shadowOpt) { shadowOpt = _shadowOpt;} 
  inline void setAaOption(const int _aaOpt) { aaOpt = _aaOpt;} 
  
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
	const Object * getObjectIntersected(const Vec3Df &camPos, const Vec3Df &dir,
			Vec3Df &intersectionPoint, Triangle &intersectionTriangle) const;

	Vec3Df getNormalAtIntersection(const Object &o, const Vec3Df &intersectionPoint,
										const Triangle &triangle) const;

	Vec3Df getPhongBRDF(const Ray &ray, const Object &o,
						const Vec3Df &intersectionPoint, const Vec3Df &normal) const;

	Vec3Df getColorFromRay(const Vec3Df &camPos, const Vec3Df &dir) const;
  
	Vec3Df getColor(const Vec3Df &camPos, const Vec3Df &dir) const;

	Vec3Df getColorFromPixel(const Vec3Df &camPos, const Vec3Df &dir) const;
	Vec3Df getColorFromPixelWithAAx2(const Vec3Df &camPos, const Vec3Df &dir) const;
	Vec3Df getColorFromPixelWithAAx3(const Vec3Df &camPos, const Vec3Df &dir) const;
      
  float shadowRay(const Vec3Df &intersectionPoint, 
						const unsigned int nbSamples) const;    
  float hardShadowRay(const Vec3Df &intersectionPoint) const;
  
  float softShadowRay(const Vec3Df &intersectionPoint, 
						const unsigned int nbSamples) const;
	float ambientOcclusion(const Vec3Df &intersectionPoint, const Vec3Df &normal) const;
	float computeAmbientOcclusion(const Vec3Df &intersectionPoint,
		const Vec3Df &normal, unsigned int nbRay, float R, float coneAngle) const;
      
private:
    Vec3Df backgroundColor;
	unsigned int screenWidth;
	unsigned int screenHeight;
	Vec3Df upVector;
	Vec3Df rightVector;
	float tanX;
	float tanY;
  int shadowOpt;
  int aaOpt;
  bool aoOpt;
};


#endif // RAYTRACER_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
