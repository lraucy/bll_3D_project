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

#include "Ray.h"

using namespace std;

int main (int argc, char **argv)
{

    /*  cout << "Test barycentric : \n";
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
    */
  QApplication raymini (argc, argv);
  setBoubekQTStyle (raymini);
  QApplication::setStyle (new QPlastiqueStyle);
  Window * window = new Window ();
  window->setWindowTitle ("RayMini: A minimal raytracer.");
  window->show();
  raymini.connect (&raymini, SIGNAL (lastWindowClosed()), &raymini, SLOT (quit()));
  
  return raymini.exec ();
  
}

