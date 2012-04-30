// *********************************************************
// Ray Tracer Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2012 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "RayTracer.h"
#include "Ray.h"
#include "Scene.h"
#include <QProgressDialog>
#include "KdTreeElement.h"
#include <cstdlib>
#include <cmath>
#include <vector>
using namespace std;
static RayTracer * instance = NULL;

#define NB_DIR 1000
RayTracer * RayTracer::getInstance () {
	if (instance == NULL)
		instance = new RayTracer ();
	return instance;
}

void RayTracer::destroyInstance () {
	if (instance != NULL) {
		delete instance;
		instance = NULL;
	}
}

inline int clamp (float f, int inf, int sup) {
	int v = static_cast<int> (f);
	return (v < inf ? inf : (v > sup ? sup : v));
}

// POINT D'ENTREE DU PROJET.
// Le code suivant ray trace uniquement la boite englobante de la scene.
// Il faut remplacer ce code par une veritable raytracer
QImage RayTracer::render (const Vec3Df & camPos,
		const Vec3Df & direction,
		const Vec3Df & upVector,
		const Vec3Df & rightVector,
		float fieldOfView,
		float aspectRatio,
		unsigned int screenWidth,
		unsigned int screenHeight) {
	QImage image (QSize (screenWidth, screenHeight), QImage::Format_RGB888);
	Scene * scene = Scene::getInstance ();
	QProgressDialog progressDialog ("Raytracing...", "Cancel", 0, 100);
	progressDialog.show ();
	for (unsigned int i = 0; i < screenWidth; i++) {
		progressDialog.setValue ((100*i)/screenWidth);
		for (unsigned int j = 0; j < screenHeight; j++) {
			float tanX = tan (fieldOfView)*aspectRatio;
			float tanY = tan (fieldOfView);
			Vec3Df stepX = (float (i) - screenWidth/2.f)/screenWidth * tanX * rightVector;
			Vec3Df stepY = (float (j) - screenHeight/2.f)/screenHeight * tanY * upVector;
			Vec3Df step = stepX + stepY;
			Vec3Df dir = direction + step;
			dir.normalize ();
			Vec3Df c (backgroundColor);

			Triangle intersectionTriangle;
			Vec3Df intersectionPoint;
			const Object * objectIntersected = NULL;

			objectIntersected = getObjectIntersected(scene, camPos, dir, intersectionPoint, intersectionTriangle);

			if(objectIntersected != NULL){
			Ray ray (camPos-objectIntersected->getTrans (), dir);
			Vec3Df normal = getNormalAtIntersection(*objectIntersected, intersectionPoint, intersectionTriangle);
	 
			Vertex  intP(intersectionPoint, normal);
			   c = ambientOcclusion(scene, ray, *objectIntersected,intP);
			// c = pathTracer(scene, ray, *objectIntersected,intP, 0);
			// c = getPhongBRDF(scene, ray, *objectIntersected, intersectionPoint, normal);
			}
			image.setPixel (i, j, qRgb (clamp (c[0], 0, 255), clamp (c[1], 0, 255), clamp (c[2], 0, 255)));
		}
	}
	progressDialog.setValue (100);
	return image;
}


const Object * RayTracer::getObjectIntersected(const Scene * scene, const Vec3Df &camPos,
									const Vec3Df &dir, Vec3Df &intersectionPoint,
									Triangle &intersectionTriangle) const{

	float smallestIntersectionDistance = 1000000.f;
	const Object *objectIntersected = NULL;

	for (unsigned int k = 0; k < scene->getObjects().size (); k++) {
		const Object & o = scene->getObjects()[k];
		Ray ray (camPos-o.getTrans (), dir);

		if(ray.intersect(o.getMesh(), o.getMesh().kdTree, intersectionPoint, smallestIntersectionDistance, intersectionTriangle))
			objectIntersected = &o;
	}
	return objectIntersected;
}

Vec3Df RayTracer::getNormalAtIntersection(const Object &o, const Vec3Df &intersectionPoint, const Triangle &triangle) const{
	const Vertex & v0 = o.getMesh().getVertices()[triangle.getVertex(0)];
	const Vertex & v1 = o.getMesh().getVertices()[triangle.getVertex(1)];
	const Vertex & v2 = o.getMesh().getVertices()[triangle.getVertex(2)];
	Vec3Df triangleCoo[3] = {v0.getPos(), v1.getPos(), v2.getPos()};
	Vec3Df barycentricCooIntersection = Triangle::getBarycentricCoo(triangleCoo,intersectionPoint);

	Vec3Df normal = barycentricCooIntersection[0]*v0.getNormal() +
					barycentricCooIntersection[1]*v1.getNormal() +
					barycentricCooIntersection[2]*v2.getNormal();
	normal.normalize();
	return normal;
}

Vec3Df RayTracer::getPhongBRDF(const Scene * scene, const Ray &ray, const Object &o, const Vec3Df &intersectionPoint, const Vec3Df &normal) const{
  
  std::vector<Light> lights = scene->getLights();
  Vec3Df colorVect = o.getMaterial().getColor();
  float color;

   for(unsigned i=0; i<lights.size(); i++){


        Vec3Df toLight = lights[i].getPos(); - intersectionPoint;
	toLight.normalize();
	Vec3Df r = (ray.getOrigin() -intersectionPoint);
	r.normalize();
	Vec3Df ref = 2*Vec3Df::dotProduct(normal, toLight)*normal - toLight;
	ref.normalize();

	float diffuseRef = o.getMaterial().getDiffuse();
	float specRef = o.getMaterial().getSpecular();
	float shininess = 16.0f;

	color += specRef * pow(std::max(Vec3Df::dotProduct(ref, r), 0.0f), shininess) + diffuseRef*std::max(Vec3Df::dotProduct(toLight, normal), 0.0f);
     }
        color = color*255;
	return Vec3Df(color*colorVect[0], color*colorVect[1], color*colorVect[2]);
}


Vec3Df RayTracer::getPhongBRDFwithLights(const Scene * scene, std::vector<Light> lights, const Ray &ray, const Object &o, const Vec3Df &intersectionPoint, const Vec3Df &normal) const{
  Vec3Df colorVect = o.getMaterial().getColor();
  float color;

  for(unsigned i=0; i<lights.size(); i++){


        Vec3Df toLight = lights[i].getPos(); - intersectionPoint;
	toLight.normalize();
	Vec3Df r = (ray.getOrigin() -intersectionPoint);
	r.normalize();
	Vec3Df ref = 2*Vec3Df::dotProduct(normal, toLight)*normal - toLight;
	ref.normalize();

	float diffuseRef = o.getMaterial().getDiffuse();
	float specRef = o.getMaterial().getSpecular();
	float shininess = 16.0f;

	color += specRef * pow(std::max(Vec3Df::dotProduct(ref, r), 0.0f), shininess) + diffuseRef*std::max(Vec3Df::dotProduct(toLight, normal), 0.0f);
     }
   // color /=lights.size();
        color = color*255;
	return Vec3Df(color*colorVect[0], color*colorVect[1], color*colorVect[2]);

}





#define MAX_DEPTH 2

Vec3Df RayTracer::pathTracer(const Scene * scene, const Ray & ray, const Object & intersectedObject, Vertex & intP, unsigned int depth) const{

  Vec3Df color(0.0,0.0,0.0);
  std::vector<Light> pathTracingLights;
  const Material &mat = intersectedObject.getMaterial();

  if(depth == MAX_DEPTH){
    color =  getPhongBRDF(scene, ray, intersectedObject, intP.getPos(), intP.getNormal());
    return color/(pow(2,depth));
  }

  vector<Vec3Df> direction = getPathTracingDirection(intP);
  Vec3Df pos = intP.getPos() + intersectedObject.getTrans();

  for(int i = 0; i<direction.size(); i++){
        const Object * intObject = NULL;
        Vec3Df intersectionPoint;
        Vec3Df lightColor(backgroundColor);
        Vec3Df intPos (pos + 10*direction[i]);
	Triangle intersectionTriangle;

	intObject = getObjectIntersected(scene,pos,direction[i],intersectionPoint,intersectionTriangle);
   
	if(intObject != NULL){
	  Ray newRay(pos, direction[i]);
	  Vec3Df normal = getNormalAtIntersection(*intObject,intersectionPoint,intersectionTriangle);
	  Vertex newIntVertex(intersectionPoint, normal);
	  lightColor = this->pathTracer(scene, newRay, *intObject, newIntVertex, depth+1);
	  intPos = newIntVertex.getPos() + intObject->getTrans();
        }
	pathTracingLights.push_back(Light(intPos, lightColor, 1.0));
  }
         color =  getPhongBRDF(scene, ray, intersectedObject, intP.getPos(), intP.getNormal()) / (scene->getLights().size());
	 color += getPhongBRDFwithLights(scene, pathTracingLights, ray, intersectedObject, intP.getPos(), intP.getNormal())/pathTracingLights.size();
         color /= 2.0*(float)(depth+1);

return color;
}

std::vector<Vec3Df> RayTracer::getPathTracingDirection( Vertex & v) const{

  std::vector<Vec3Df> directions;

    Vec3Df normal = v.getNormal();

    for (unsigned int i=0; i<NB_DIR; i++) {

        Vec3Df randomDirection;
        for (int j=0; j<3; j++) {

	  randomDirection[j] += (float)(rand()) / (float)(RAND_MAX) - 0.5; // in [-0.5,0.5]

        }
        randomDirection.normalize();

	randomDirection *= 0.4;
        randomDirection += normal;

	randomDirection.normalize();
	directions.push_back(randomDirection);
    }

    return directions;
}

Vec3Df RayTracer::ambientOcclusion(Scene * scene, const Ray & ray, const Object & intersectedObject, Vertex & intP)const{


  vector<Vec3Df> direction = getPathTracingDirection(intP);
  Vec3Df pos = intP.getPos() + intersectedObject.getTrans();
  Vec3Df color(0.0,0.0,0.0);
  Vec3Df white(255.0,255.0,255.0);
	float a = 1.0;
	float b=0.0;
	float c=0.5;
for(int i = 0; i<direction.size(); i++){
   const Object * intObject = NULL;
        Vec3Df intPos;
	Triangle intersectionTriangle;
  
	intObject = getObjectIntersected(scene,pos,direction[i],intPos,intersectionTriangle);
	float d =  sqrt((intPos[0]-pos[0])*(intPos[0]-pos[0]) +(intPos[1]-pos[1])*(intPos[1]-pos[1])+(intPos[2]-pos[2])*(intPos[2]-pos[2]));
	  if(intObject == NULL){
	  float intensity = 1/(a+b*d+c*pow(d,2));
	  color+=(white*Vec3Df::dotProduct(intP.getNormal(),direction[i]))*intensity;
        }
  }
 color/=direction.size();

 //color[0] = int(color[0]/8.0)*8.0;
 //color[1] = int(color[1]/8.0)*8.0;
 //color[2] = int(color[2]/8.0)*8.0;
 return color;
}
