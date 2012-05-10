// *********************************************************
// Scene Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include <vector>

#include "Object.h"
#include "Light.h"
#include "BoundingBox.h"

#define SCENE_DEFAULT_SCENE 0
#define SCENE_FINAL_SCENE 1
#define SCENE_CLOSED_BOX_SCENE 2
#define SCENE_CLOSED_BOX_PATH_TRACING_SCENE 3
#define SCENE_DOUBLE_LIGHT_SCENE 4

class Scene {
public:
    static Scene * getInstance ();
    static void destroyInstance ();
    
    inline std::vector<Object> & getObjects () { return objects; }
    inline const std::vector<Object> & getObjects () const { return objects; }
    
    inline std::vector<Light> & getLights () { return lights; }
    inline const std::vector<Light> & getLights () const { return lights; }
    
    inline const BoundingBox & getBoundingBox () const { return bbox; }
    void updateBoundingBox ();
	void rebuildKdTree();

    void switchScene(unsigned int scene);
protected:
    Scene ();
    virtual ~Scene ();
    
private:
    void buildDefaultScene ();
    void buildClosedBoxScene ();
    void buildFinalScene ();
    void buildDoubleLightScene ();
    void buildClosedBoxPathTracingScene ();
    std::vector<Object> objects;
    std::vector<Light> lights;
    BoundingBox bbox;
};


#endif // SCENE_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
