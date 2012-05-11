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
  rebuildKdTree();
  updateBoundingBox ();
}

Scene::~Scene () {
}

void Scene::switchScene(unsigned int scene) {
  if(&objects != NULL)
    objects.erase(objects.begin(), objects.end());
  if(&lights != NULL)
    lights.erase(lights.begin(), lights.end());

  switch(scene) {
  case SCENE_DEFAULT_SCENE:
    buildDefaultScene ();
    break;
  case SCENE_FINAL_SCENE:
    buildFinalScene ();
    break;
  case SCENE_CLOSED_BOX_SCENE:
    buildClosedBoxScene ();
    break;
  case SCENE_CLOSED_BOX_PATH_TRACING_SCENE:
    buildClosedBoxPathTracingScene ();
    break;
  case SCENE_DOUBLE_LIGHT_SCENE:
    buildDoubleLightScene ();
    break;
  default:
    buildDefaultScene ();
    break;
  }

  rebuildKdTree();
  updateBoundingBox ();
}


void Scene::rebuildKdTree() {
	for (unsigned int i = 0; i < objects.size(); i++) {
		delete objects[i].getMesh().kdTree;
		objects[i].getMesh().buildKdTree();
	}
	
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


void Scene::buildClosedBoxScene () {
  Mesh wallLeftMesh;
  wallLeftMesh.loadOFF ("models/WallLeft.off");
  Material wallLeftMat(0.8f, 0.2f, Vec3Df(0.f, 1.f, 1.f), 1.f) ;
  Object wallLeft (wallLeftMesh, wallLeftMat, QString("Wall left"));    
  wallLeft.setTrans(Vec3Df(-2.f, -2.f, 0.f));
  objects.push_back (wallLeft);
   

  Mesh wallRightMesh;
  wallRightMesh.loadOFF ("models/WallRight.off");
  Material wallRightMat(0.8f, 0.2f, Vec3Df(1.f, 0.f, 1.f), 1.f) ;
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
    
  Mesh rhinoMesh;
  rhinoMesh.loadOFF ("models/rhino.off");
  Material rhinoMat (1.0f, 0.2f, Vec3Df (0.6f, 0.6f, 0.7f));
  Object rhino (rhinoMesh, rhinoMat, QString("Rhino"));
  rhino.setTrans (Vec3Df (0.f, .0f, 0.4f));
  objects.push_back (rhino);
    
    
  Light l (Vec3Df (0.0f, 0.0f, 4.9f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f, 0.2f, Vec3Df (-2.0f, -2.f, 0.0f));
  lights.push_back (l);
}

void Scene::buildDoubleLightScene () {
  Mesh wallLeftMesh;
  wallLeftMesh.loadOFF ("models/WallLeft.off");
  Material wallLeftMat(0.8f, 0.2f, Vec3Df(1.f, 1.f, 1.f)) ;
  Object wallLeft (wallLeftMesh, wallLeftMat, QString("Wall left"));    
  wallLeft.setTrans(Vec3Df(-2.f, -2.f, 0.f));
  objects.push_back (wallLeft);
   
  Mesh wallRightMesh;
  wallRightMesh.loadOFF ("models/WallRight.off");
  Material wallRightMat(0.8f, 0.2f, Vec3Df(1.f, 1.f, 1.f)) ;
  Object wallRight (wallRightMesh, wallRightMat, QString("Wall right"));    
  wallRight.setTrans(Vec3Df(2.f, -2.f, 0.f));
  objects.push_back (wallRight);
      
  Mesh wallBackMesh;
  wallBackMesh.loadOFF ("models/WallBack.off");
  Material wallBackMat(0.8f, 0.2f, Vec3Df(1.f, 1.f, 1.f)) ;
  Object wallBack (wallBackMesh, wallBackMat, QString("Wall back"));    
  wallBack.setTrans(Vec3Df(-2.f, 2.f, 0.f));
  objects.push_back (wallBack);

  Mesh wallFrontMesh;
  wallFrontMesh.loadOFF ("models/WallFront.off");
  Material wallFrontMat(0.8f, 0.2f, Vec3Df(1.f, 1.f, 1.f)) ;
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
    
  Mesh rhinoMesh;
  rhinoMesh.loadOFF ("models/rhino.off");
  Material rhinoMat (1.0f, 0.2f, Vec3Df (0.6f, 0.6f, 0.7f));
  Object rhino (rhinoMesh, rhinoMat, QString("Rhino"));
  rhino.setTrans (Vec3Df (0.f, .0f, 0.4f));
  objects.push_back (rhino);
        
  Light l1 (Vec3Df (-2.f, -2.f, 4.9f), Vec3Df (1.0f, 0.0f, 0.f), 1.0f, 0.2f, Vec3Df (2.0f, 2.f, 0.0f));
  lights.push_back (l1);
  Light l2 (Vec3Df (2.f, 2.f, 4.9f), Vec3Df (0.0f, 0.0f, 1.f), 1.0f, 0.2f, Vec3Df (-2.0f, -2.f, 0.0f));
  lights.push_back (l2);
}

void Scene::buildClosedBoxPathTracingScene () {

  Mesh sphereMesh;
  sphereMesh.loadOFF("models/sphere2.off");
  Material sphereMat(0.7f, 0.4f,  Vec3Df (0.5f, 0.8f, 1.f));
  Object sphere(sphereMesh, sphereMat);
  sphere.setTrans(Vec3Df(0.f, 0.f, 1.f));
  objects.push_back(sphere);
  
  Mesh wallLeftMesh;
  wallLeftMesh.loadOFF ("models/WallLeft.off");
  Material wallLeftMat(0.8f, 0.2f, Vec3Df(0.f, 1.f, 1.f)) ;
  Object wallLeft (wallLeftMesh, wallLeftMat, QString("Wall left"));    
  wallLeft.setTrans(Vec3Df(-2.f, -2.f, 0.f));
  objects.push_back (wallLeft);
   

  Mesh wallRightMesh;
  wallRightMesh.loadOFF ("models/WallRight.off");
  Material wallRightMat(0.8f, 0.2f, Vec3Df(1.f, 0.f, 1.f)) ;
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
  Material topMat(0.8, 0.2, Vec3Df(0.5,0.5,0.5), 0.0, Vec3Df(120.0, 120.0, 120.0));
  Object top (topMesh, topMat, QString("Wall top"));    
  top.setTrans(Vec3Df(0.f, 0.f, 5.f));
  objects.push_back (top);  
  
  Mesh groundMesh;
  groundMesh.loadOFF ("models/ground.off");
  Material groundMat;
  Object ground (groundMesh, groundMat, QString("Ground"));    
  objects.push_back (ground);  

  Light l (Vec3Df (0.0f, 0.0f, 4.9f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f);
  lights.push_back (l);
}

void Scene::buildFinalScene () {
  Mesh letterMesh;
  letterMesh.loadOFF ("models/D.off");
  Material letterMat(0.8f, 0.2f, Vec3Df(0.f, 1.f, 1.f)) ;
  Object letter (letterMesh, letterMat, QString("letter"));    
  letter.setTrans(Vec3Df(0.0f, 1.0f, 0.0f));
  objects.push_back (letter);
  
  Mesh numberMesh;
  numberMesh.loadOFF ("models/3.off");
  Material numberMat(0.8f, 0.2f, Vec3Df(0.f, 1.f, 1.f)) ;
  Object number (numberMesh, numberMat, QString("number"));    
  number.setTrans(Vec3Df(-4.0f, 1.0f, 0.0f));
  objects.push_back (number);

  Mesh sphereMesh;
  sphereMesh.loadOFF("models/sphere2.off");
  Material sphereMat(0.7f, 0.4f, Vec3Df(1.f, 1.f, 1.f), 1.0f);
  Object sphere(sphereMesh, sphereMat);
  sphere.setTrans(Vec3Df(3.f, -3.f, 1.f));
  objects.push_back(sphere);
 
  
  Mesh wallRightMesh;
  wallRightMesh.loadOFF ("models/WallRightModified.off");
  Material wallRightMat(0.8f, 0.2f, Vec3Df(0.f, 0.f, 1.f)) ;
  Object wallRight (wallRightMesh, wallRightMat, QString("Wall right"));    
  wallRight.setTrans(Vec3Df(2.f, -2.f, 0.f));
  objects.push_back (wallRight);
  
  
  Mesh wallBackMesh;
  wallBackMesh.loadOFF ("models/WallBackModified.off");
  Material wallBackMat(0.8f, 0.2f, Vec3Df(1.f, 0.f, 0.f), 1.0f) ;
  Object wallBack (wallBackMesh, wallBackMat, QString("Wall back"));    
  wallBack.setTrans(Vec3Df(-2.f, 2.f, 0.f));
  objects.push_back (wallBack);
  
  Mesh groundMesh;
  groundMesh.loadOFF ("models/groundModified.off");
  Material groundMat;
  Object ground (groundMesh, groundMat, QString("Ground"));    
  objects.push_back (ground);  
    
  Mesh ramMesh;
  ramMesh.loadOFF ("models/ram.off");
  Material ramMat (1.f, 1.f, Vec3Df (1.f, .6f, .2f));
  Object ram (ramMesh, ramMat, QString("Biquette"));
  ram.setTrans (Vec3Df (-0.5f, -0.5f, 0.f));
  objects.push_back (ram);
    
  Mesh rhinoMesh;
  rhinoMesh.loadOFF ("models/rhino.off");
  Material rhinoMat (1.0f, 0.2f, Vec3Df (0.6f, 0.6f, 0.7f));
  Object rhino (rhinoMesh, rhinoMat, QString("Rhino"));
  rhino.setTrans (Vec3Df (-2.3f, 0.5f, 1.2f));
  objects.push_back (rhino);
    
  Mesh gargMesh;
  gargMesh.loadOFF ("models/gargoyle.off");
  Material gargMat (0.7f, 0.4f, Vec3Df (0.5f, 0.8f, 0.5f), 0.0f, Vec3Df(0,0,0), true, 0.7f, 0.3f, 1.3f);
  Object garg (gargMesh, gargMat, QString("Gargoyle"));
  garg.setTrans (Vec3Df (-2.3f, -0.4f, 3.35f));
  objects.push_back (garg);
        
  Light l (Vec3Df (0.0f, -7.f, 7.f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f, 0.2f, Vec3Df (.0f, 7.f, -7.0f));
  lights.push_back (l);

}

void Scene::buildDefaultScene () {
    Mesh groundMesh;
    groundMesh.loadOFF ("models/ground.off");
    Material groundMat;
    Object ground (groundMesh, groundMat);    
    objects.push_back (ground);
    Mesh ramMesh;
    ramMesh.loadOFF ("models/ram.off");
    Material ramMat (1.f, 1.f, Vec3Df (1.f, .6f, .2f));
    Object ram (ramMesh, ramMat);
    ram.setTrans (Vec3Df (1.f, 0.5f, 0.f));
    objects.push_back (ram);
    Mesh rhinoMesh;
    rhinoMesh.loadOFF ("models/rhino.off");
    Material rhinoMat (1.0f, 0.2f, Vec3Df (0.6f, 0.6f, 0.7f));
    Object rhino (rhinoMesh, rhinoMat);
    rhino.setTrans (Vec3Df (-1.f, -1.0f, 0.4f));
    objects.push_back (rhino);
    Mesh gargMesh;
    gargMesh.loadOFF ("models/gargoyle.off");
    Material gargMat (0.7f, 0.4f, Vec3Df (0.5f, 0.8f, 0.5f));
    Object garg (gargMesh, gargMat);
    garg.setTrans (Vec3Df (-1.f, 1.0f, 0.1f));
    objects.push_back (garg);
    Light l (Vec3Df (3.0f, 3.0f, 3.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f, 0.2f, Vec3Df (-3.0f, -3.0f, -3.0f));
    lights.push_back (l);
}
