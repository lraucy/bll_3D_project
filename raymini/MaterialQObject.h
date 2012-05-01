#ifndef MATERIAL_QOBJECT_H
#define MATERIAL_QOBJECT_H

#include "Material.h"
#include <QObject>

class MaterialQObject : public QObject {

	Q_OBJECT

public:
	inline MaterialQObject(Material* material): material(material) {}
public slots:
	virtual void setReflectance(double reflectance);
private:
		Material *material;
};

#endif
