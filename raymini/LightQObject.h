#ifndef QLIGHT_H
#define QLIGHT_H

#include "Light.h"
#include <QObject>

class LightQObject : public QObject {

	Q_OBJECT

public:
	inline LightQObject(Light* light): light(light) {}
public slots:
	virtual void setIntensity(double intensity);
	virtual void setRed(double val);
	virtual void setGreen(double val);
	virtual void setBlue(double val);
private:
		Light *light;
};

#endif
