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

static RayTracer * instance = NULL;

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
            Vec3Df intersectionPoint;
            float smallestIntersectionDistance = 1000000.f;
            Vec3Df c (backgroundColor);

	    bool hasIntersection = false;
	    Triangle triangle;
	    unsigned int objectIntersected;

            for (unsigned int k = 0; k < scene->getObjects().size (); k++) {
                const Object & o = scene->getObjects()[k];
                Ray ray (camPos-o.getTrans (), dir);
		
		if(ray.intersect(o.getMesh(), o.getMesh().kdTree, intersectionPoint, smallestIntersectionDistance, triangle)){
		  hasIntersection = true;
		  objectIntersected = k;
		}
	    }
	    if(hasIntersection){
			const Object & o = scene->getObjects()[objectIntersected];
			const Vertex & v0 = o.getMesh().getVertices()[triangle.getVertex(0)];
			const Vertex & v1 = o.getMesh().getVertices()[triangle.getVertex(1)];
			const Vertex & v2 = o.getMesh().getVertices()[triangle.getVertex(2)];
			Vec3Df triangleCoo[3] = {v0.getPos(), v1.getPos(), v2.getPos()};
			Ray ray (camPos-o.getTrans (), dir);
	      
			Vec3Df barycentricCoo = Triangle::getBarycentricCoo(triangleCoo, intersectionPoint);
    
			Vec3Df normal = barycentricCoo[0]*v0.getNormal() +
							barycentricCoo[1]*v1.getNormal() +
							barycentricCoo[2]*v2.getNormal();
		
		normal.normalize();
		Vec3Df lightVector = intersectionPoint - scene->getLights()[0].getPos();
		lightVector.normalize();
		float w_i = std::max(Vec3Df::dotProduct(normal, lightVector), (float) 0);
		
		Vec3Df colorVect;
		float diffuseRef = o.getMaterial().getDiffuse();
		float specRef = o.getMaterial().getSpecular();
		float shininess = 16.0f;
	         
		colorVect = o.getMaterial().getColor();
		
		float color = diffuseRef*w_i + specRef*pow(std::max(Vec3Df::dotProduct(Vec3Df::crossProduct(normal, lightVector), ray.getDirection()), (float)0.0), shininess);
		
		color = 255-color*255;
		c = Vec3Df(color*colorVect[0], color*colorVect[1], color*colorVect[2]);

	    }
            image.setPixel (i, j, qRgb (clamp (c[0], 0, 255), clamp (c[1], 0, 255), clamp (c[2], 0, 255)));
        }
    }
    progressDialog.setValue (100);
    return image;
}
