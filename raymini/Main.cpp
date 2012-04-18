#include <QApplication>
#include "Window.h"
#include <QDir>
#include <QPixmap>
#include <QSplashScreen>
#include <QPlastiqueStyle>
#include <QCleanlooksStyle>
#include <string>
#include <iostream>

#include "QTUtils.h"

using namespace std;


// Include for tests
#include "Ray.h"
#include "KDTreeBuilder.h"
#include "BoundingBox.h"
#include "Triangle.h"
#include "Mesh.h"
#include "Triangle.h"



void testKDTree () {
    BoundingBox b;
    Mesh mesh;
    vector<unsigned int> triangles;

    b = BoundingBox(Vec3Df(0, 0, 0), Vec3Df(10, 10, 10));

    vector<Vertex> v = vector<Vertex>();
    v.push_back(Vertex(Vec3Df(5, 2, 0)));
    v.push_back(Vertex(Vec3Df(6, 2, 0)));
    v.push_back(Vertex(Vec3Df(5.5, 0, 1)));
    v.push_back(Vertex(Vec3Df(0, 4, 5)));
    v.push_back(Vertex(Vec3Df(0, 5, 5)));
    v.push_back(Vertex(Vec3Df(1, 5, 5)));
    v.push_back(Vertex(Vec3Df(1, 1, 0)));
    v.push_back(Vertex(Vec3Df(3, 1, 0)));
    v.push_back(Vertex(Vec3Df(3, 1, 5)));
    v.push_back(Vertex(Vec3Df(3, 4, 3))); 
    v.push_back(Vertex(Vec3Df(3, 6, 2))); 
    v.push_back(Vertex(Vec3Df(3, 5, 1)));
    v.push_back(Vertex(Vec3Df(9, 7, 0))); 
    v.push_back(Vertex(Vec3Df(9, 8, 0))); 
    v.push_back(Vertex(Vec3Df(9, 7.5, 1))); 
    v.push_back(Vertex(Vec3Df(1, 8, 2))); 
    v.push_back(Vertex(Vec3Df(1, 9, 2))); 
    v.push_back(Vertex(Vec3Df(1, 8, 3)));
 
    vector<Triangle> t = vector<Triangle>();
    t.push_back(Triangle(0, 1, 2));
    t.push_back(Triangle(3, 4, 5));
    t.push_back(Triangle(6, 7, 8));
    t.push_back(Triangle(9, 10, 11));
    t.push_back(Triangle(12, 13, 14));
    t.push_back(Triangle(15, 16, 17));
    mesh = Mesh(v, t);

    KDTreeBuilder builder = KDTreeBuilder ();

    triangles = vector<unsigned int>();
    triangles.push_back(0);
    triangles.push_back(1);
    triangles.push_back(2);
    triangles.push_back(3);
    triangles.push_back(4);
    triangles.push_back(5);

    cout << "Number of triangles : " << triangles.size() << "\n";
    
    KDTreeNode tree = builder.buildKDTreeRec (b, mesh, triangles);

    //tree.print();
    
}

void testBarycentric () {
    cout << "Test barycentric : \n";
    Ray r = Ray(Vec3Df(0, 0, 4), Vec3Df(0, 0, -1));
    Vec3Df intersectionPoint;
    Vec3Df barycentricCoo;
    Vec3Df triangle[3] = 
	{
	    Vec3Df(-1,-1,-1),
	    Vec3Df(1,-1,-1),
	    Vec3Df(0,1,-1) };

    r.intersectTriangle(triangle, intersectionPoint, barycentricCoo);

    cout << "intersect in " << intersectionPoint << "\n";
    cout << "Bary " << barycentricCoo << "\n";
}

int main (int argc, char **argv)
{

    
    testKDTree ();
    /*    QApplication raymini (argc, argv);
    setBoubekQTStyle (raymini);
    QApplication::setStyle (new QPlastiqueStyle);
    Window * window = new Window ();
    window->setWindowTitle ("RayMini: A minimal raytracer.");
    window->show();
    raymini.connect (&raymini, SIGNAL (lastWindowClosed()), &raymini, SLOT (quit()));
  
    return raymini.exec ();
    */
}

