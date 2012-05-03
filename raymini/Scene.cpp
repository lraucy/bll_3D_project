// *********************************************************
// Scene Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Scene.h"

using namespace std;

static Scene * instance = NULL;

Scene * Scene::getInstance () {
  if (instance == NULL)
    instance = new Scene ();
  return instance;
}

void Scene::destroyInstance () {
  if (instance != NULL) {
    delete instance;
    instance = NULL;
  }
}

Scene::Scene () {
  buildDefaultScene ();
  updateBoundingBox ();
}

Scene::~Scene () {
}

void Scene::updateBoundingBox () {
  if (objects.empty ())
    bbox = BoundingBox ();
  else {
    bbox = objects[0].getBoundingBox ();
    for (unsigned int i = 1; i < objects.size (); i++)
      bbox.extendTo (objects[i].getBoundingBox ());
  }
}

// Changer ce code pour creer des scenes originales
void Scene::buildDefaultScene () {
  /*
    Mesh sphereMesh;
    sphereMesh.loadOFF("models/sphere.off");
    Material sphereMat(0.7f, 0.4f, 0.0f, Vec3Df (0.5f, 0.8f, 0.5f));
    Object sphere(sphereMesh, sphereMat);
    objects.push_back(sphere);
  */

  
  Mesh wallLeftMesh;
  wallLeftMesh.loadOFF ("models/WallLeft.off");
  Material wallLeftMat(0.8f, 0.2f, Vec3Df(0.f, 1.f, 1.f)) ;
  Object wallLeft (wallLeftMesh, wallLeftMat, QString("Wall left"));    
  wallLeft.setTrans(Vec3Df(-2.f, -2.f, 0.f));
  objects.push_back (wallLeft);
   

  Mesh wallRightMesh;
  wallRightMesh.loadOFF ("models/WallRight.off");
  Material wallRightMat(0.8f, 0.2f, Vec3Df(1.f, 0.f, 1.f), 0.0f, Vec3Df(255.0, 255.0, 255.0)) ;
  Object wallRight (wallRightMesh, wallRightMat, QString("Wall right"));    
  wallRight.setTrans(Vec3Df(2.f, -2.f, 0.f));
  objects.push_back (wallRight);
      
  Mesh wallBackMesh;
  wallBackMesh.loadOFF ("models/WallBack.off");
  Material wallBackMat(0.8f, 0.2f, Vec3Df(1.f, 1.f, 0.f)) ;
  Object wallBack (wallBackMesh, wallBackMat, QString("Wall back"));    
  wallBack.setTrans(Vec3Df(-2.f, 2.f, 0.f));
  objects.push_back (wallBack);

  Mesh wallFrontMesh;
  wallFrontMesh.loadOFF ("models/WallFront.off");
  Material wallFrontMat(0.8f, 0.2f, Vec3Df(1.f, 1.f, 0.f)) ;
  Object wallFront (wallFrontMesh, wallFrontMat, QString("Wall front"));    
  wallFront.setTrans(Vec3Df(-2.f, -2.f, 0.f));
  objects.push_back (wallFront);
    

  Mesh topMesh;
  topMesh.loadOFF ("models/top.off");
  Material topMat;
  Object top (topMesh, topMat, QString("Wall top"));    
  top.setTrans(Vec3Df(0.f, 0.f, 5.f));
  objects.push_back (top);  
  
  Mesh groundMesh;
  groundMesh.loadOFF ("models/ground.off");
  Material groundMat;
  Object ground (groundMesh, groundMat, QString("Ground"));    
  objects.push_back (ground);  
    
  Mesh ramMesh;
  ramMesh.loadOFF ("models/ram.off");
  Material ramMat (1.f, 1.f, Vec3Df (1.f, .6f, .2f));
  Object ram (ramMesh, ramMat, QString("Biquette"));
  ram.setTrans (Vec3Df (1.f, 0.5f, 0.f));
  objects.push_back (ram);
    
  Mesh rhinoMesh;
  rhinoMesh.loadOFF ("models/rhino.off");
  Material rhinoMat (1.0f, 0.2f, Vec3Df (0.6f, 0.6f, 0.7f));
  Object rhino (rhinoMesh, rhinoMat, QString("Rhino"));
  rhino.setTrans (Vec3Df (-1.f, -1.0f, 0.4f));
  objects.push_back (rhino);
    
  Mesh gargMesh;
  gargMesh.loadOFF ("models/gargoyle.off");
  Material gargMat (0.7f, 0.4f, Vec3Df (0.5f, 0.8f, 0.5f), 0.0f, Vec3Df(0,0,0), true, 0.5f, 0.5f);
  Object garg (gargMesh, gargMat, QString("Gargoyle"));
  garg.setTrans (Vec3Df (-1.f, 1.0f, 0.1f));
  objects.push_back (garg);
    
  for (unsigned int i = 0; i < objects.size(); i++)
    objects[i].getMesh().buildKdTree();
    
  Light l (Vec3Df (0.0f, 0.0f, 4.9f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f);
  lights.push_back (l);
  Light l2 (Vec3Df (-3.0f, -3.0f, 3.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f);
	      //lights.push_back (l2);

}
