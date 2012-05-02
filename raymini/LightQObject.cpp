#include "LightQObject.h"

void LightQObject::setIntensity(double intensity) {
	light->setIntensity(intensity);
}

void LightQObject::setRed(double val) {
  Vec3Df color = light->getColor();
  color[0] = val;
  light->setColor(color);
}
void LightQObject::setGreen(double val) {
  Vec3Df color = light->getColor();
  color[1] = val;
  light->setColor(color);
}
void LightQObject::setBlue(double val) {
  Vec3Df color = light->getColor();
  color[2] = val;
  light->setColor(color);
}
