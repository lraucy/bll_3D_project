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

#define RAYTRACER_NO_SHADOW 0
#define RAYTRACER_HARD_SHADOW 1
#define RAYTRACER_SOFT_SHADOW 2

#define RAYTRACER_NO_AA 0
#define RAYTRACER_AAx2 1
#define RAYTRACER_AAx3 2

#define RAY_TRACING 0
#define PATH_TRACING 1
#define PATH_TRACING_LOIC 2




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
  inline void setAONbRay(unsigned int _aoNbRay) { aoNbRay = _aoNbRay;}
  inline unsigned int getAONbRay() { return aoNbRay; }
  inline void setShadowNbRay(unsigned int _shadowNbRay) { shadowNbRay = _shadowNbRay;}
  inline unsigned int getShadowNbRay() { return shadowNbRay; }
  void setShadowRadius(float _shadowRadius);
  float getShadowRadius();
  inline void setAOSphereRadius(unsigned int _aoSphereRadius) { aoSphereRadius = _aoSphereRadius;}
  inline unsigned int getAOSphereRadius() { return aoSphereRadius; }
  inline void setAOConeAngle(unsigned int _aoConeAngle) { aoConeAngle = _aoConeAngle;}
  inline unsigned int getAOConeAngle() { return aoConeAngle; }
  inline void setAOCoeff(float _aoCoeff) { aoCoeff = _aoCoeff;}
  inline float getAOCoeff() { return aoCoeff; }
  
  inline void setPathTraceNbRay(unsigned int _pathTraceNbRay) { pathTraceNbRay = _pathTraceNbRay;}
  inline unsigned int getPathTraceNbRay() { return pathTraceNbRay; }
  inline void setPathTraceAngle(float _pathTraceAngle) { pathTraceAngle = _pathTraceAngle;}
  inline float getPathTraceAngle() { return pathTraceAngle; }
  inline void setPathTraceDepth(int _pathTraceDepth) { pathTraceDepth = _pathTraceDepth;}
  inline float getPathTraceDepth() { return pathTraceDepth; }

  inline unsigned int getPathTraceDepthLoic() {return pathTraceDepthLoic;}
  inline void setPathTraceDepthLoic(unsigned int pathTraceDepth_) {pathTraceDepthLoic = pathTraceDepth_;}
  inline void resetPathTracingLoic() {iterationPathTracingLoic = 0;}
  inline unsigned int getIterationPerTracingLoic() {return iterationPerTracingLoic;}
  inline void setIterationPerTracingLoic(unsigned int iteration) {iterationPerTracingLoic = iteration;}

  inline void setTracing(unsigned int tracing_) {tracing = tracing_;}
  
    QImage render (const QImage &image_,
				   const Vec3Df & camPos,
                   const Vec3Df & viewDirection,
                   const Vec3Df & upVector,
                   const Vec3Df & rightVector,
                   float fieldOfView,
                   float aspectRatio,
                   unsigned int screenWidth,
                   unsigned int screenHeight);
    
protected:
    inline RayTracer () { aoNbRay = 10; aoSphereRadius = 5; aoConeAngle = 70; aoCoeff = 0.2;
	  shadowNbRay = 10; aoOpt = false; pathTraceNbRay = 5; pathTraceAngle = M_PI/2; pathTraceDepth = 2;
	  iterationPathTracingLoic = 0; pathTraceDepthLoic = 4; iterationPerTracingLoic = 10;}
    inline virtual ~RayTracer () {}
	const Object * getObjectIntersected(const Vec3Df &camPos, const Vec3Df &dir,
			Vec3Df &intersectionPoint, Triangle &intersectionTriangle) const;

	Vec3Df getNormalAtIntersection(const Object &o, const Vec3Df &intersectionPoint,
										const Triangle &triangle) const;
	Vec3Df getPhongBRDF(const Vec3Df &eyeVector, const Vec3Df &lightVector,
			const Vec3Df &normal, const Material &material) const;


	Vec3Df getPhongBRDF(const Ray &ray, const Object &o,
						const Vec3Df &intersectionPoint, const Vec3Df &normal) const;

	Vec3Df getPhongBRDFWithLights(const Ray &ray, const Object &o,
								  const Vec3Df &intersectionPoint, const Vec3Df &normal, const std::vector<Light> lights) const;

	Vec3Df getPhongBRDFReflectance(const Ray &ray, const Object &o,
						const Vec3Df &intersectionPoint, const Vec3Df &normal) const;

	Vec3Df getColorFromRay(const Vec3Df &camPos, const Vec3Df &dir) const;
	Vec3Df getColorFromRayWithRayTracing(const Vec3Df &camPos, const Vec3Df &dir) const;
  
	Vec3Df getColor(const Vec3Df &camPos, const Vec3Df &dir) const;

	Vec3Df getColorFromPixel(const Vec3Df &camPos, const Vec3Df &dir) const;
	Vec3Df getColorFromPixelWithAAx2(const Vec3Df &camPos, const Vec3Df &dir) const;
	Vec3Df getColorFromPixelWithAAx3(const Vec3Df &camPos, const Vec3Df &dir) const;
      
  float shadowRay(const Vec3Df &intersectionPoint) const;
  float hardShadowRay(const Vec3Df &intersectionPoint) const;
  
  float softShadowRay(const Vec3Df &intersectionPoint, 
						const unsigned int nbSamples) const;
	float ambientOcclusion(const Vec3Df &intersectionPoint, const Vec3Df &normal) const;
	float computeAmbientOcclusion(const Vec3Df &intersectionPoint,
		const Vec3Df &normal, unsigned int nbRay, float R, float coneAngle) const;

  Vec3Df pathTracer(const Vec3Df &camPos, const Vec3Df &dir) const ;
  Vec3Df pathTracerRec(const Ray & ray, const Object & intersectedObject, const Vertex & intP, unsigned int depth) const;
  std::vector<Vec3Df> getRandomDirections(const Vertex & v, const float angle) const;

	Vec3Df tracePathLoic(Ray &ray, unsigned int depth) const;
	Ray * getRandomRay(const Vec3Df &origin, const Vec3Df &normal) const;
  
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
  unsigned int shadowNbRay;
  bool aoOpt;
  unsigned int aoNbRay;
  unsigned int aoSphereRadius;
  unsigned int aoConeAngle;
  float aoCoeff;
  unsigned int pathTraceNbRay;
  float pathTraceAngle;
  float pathTraceDepth;

  unsigned int tracing;
  
  unsigned int iterationPathTracingLoic;
  unsigned int pathTraceDepthLoic;
  unsigned int iterationPerTracingLoic;
  
};


#endif // RAYTRACER_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
