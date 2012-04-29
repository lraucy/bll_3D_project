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

int main (int argc, char **argv)
{
    Vec3Df dir(0.0f, -1.0f, -1.0f);
    Vec3Df normal(0.0f, 0.0f, 1.0f);

    float scal = Vec3Df::dotProduct(dir, normal);
    Vec3Df tmp = normal * scal * 2;
    tmp.normalize();
    Vec3Df reflectedRay = 2 * normal * (Vec3Df::dotProduct(-dir, normal)) + dir;//  tmp * dir;
    reflectedRay.normalize();
    std::cout << dir << " reflected " << reflectedRay << " normal is " << normal << "\n";

    
  QApplication raymini (argc, argv);
  setBoubekQTStyle (raymini);
  QApplication::setStyle (new QPlastiqueStyle);
  Window * window = new Window ();
  window->setWindowTitle ("RayMini: A minimal raytracer.");
  window->show();
  raymini.connect (&raymini, SIGNAL (lastWindowClosed()), &raymini, SLOT (quit()));
  
  return raymini.exec ();
}

