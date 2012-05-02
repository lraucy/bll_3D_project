#include <GL/glew.h>
#include "Window.h"

#include <vector>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>

#include <QDockWidget>
#include <QGroupBox>
#include <QButtonGroup>
#include <QMenuBar>
#include <QApplication>
#include <QLayout>
#include <QLabel>
#include <QProgressBar>
#include <QCheckBox>
#include <QRadioButton>
#include <QColorDialog>
#include <QLCDNumber>
#include <QPixmap>
#include <QFrame>
#include <QSplitter>
#include <QMenu>
#include <QScrollArea>
#include <QCoreApplication>
#include <QFont>
#include <QSizePolicy>
#include <QImageReader>
#include <QStatusBar>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>
#include <QStatusBar>

#include "RayTracer.h"
#include "MaterialQObject.h"

using namespace std;


Window::Window () : QMainWindow (NULL) {
    try {
        viewer = new GLViewer;
    } catch (GLViewer::Exception e) {
        cerr << e.getMessage () << endl;
        exit (1);
    }
    setCentralWidget (viewer);
    
    QDockWidget * controlDockWidget = new QDockWidget (this);
    initControlWidget ();
    
    controlDockWidget->setWidget (scrollArea);
    controlDockWidget->adjustSize ();
    addDockWidget (Qt::RightDockWidgetArea, controlDockWidget);
    controlDockWidget->setFeatures (QDockWidget::AllDockWidgetFeatures);
    statusBar()->showMessage("");
}

Window::~Window () {

}

void Window::renderRayImage () {
    qglviewer::Camera * cam = viewer->camera ();
    RayTracer * rayTracer = RayTracer::getInstance ();
    qglviewer::Vec p = cam->position ();
    qglviewer::Vec d = cam->viewDirection ();
    qglviewer::Vec u = cam->upVector ();
    qglviewer::Vec r = cam->rightVector ();
    Vec3Df camPos (p[0], p[1], p[2]);
    Vec3Df viewDirection (d[0], d[1], d[2]);
    Vec3Df upVector (u[0], u[1], u[2]);
    Vec3Df rightVector (r[0], r[1], r[2]);
    float fieldOfView = cam->fieldOfView ();
    float aspectRatio = cam->aspectRatio ();
    unsigned int screenWidth = cam->screenWidth ();
    unsigned int screenHeight = cam->screenHeight ();
    QTime timer;
    timer.start ();
	for (unsigned int i = 0; i < 5000; i++) {
		viewer->setRayImage(rayTracer->render (viewer->getRayImage(), i, camPos, viewDirection, upVector, rightVector,
							fieldOfView, aspectRatio, screenWidth, screenHeight));
		statusBar()->showMessage(QString ("Raytracing performed in ") +
								 QString::number (timer.elapsed ()) +
								 QString ("ms at ") +
								 QString::number (screenWidth) + QString ("x") + QString::number (screenHeight) +
								 QString (" screen resolution"));
		viewer->setDisplayMode (GLViewer::RayDisplayMode);
	}
}

void Window::setBGColor () {
    QColor c = QColorDialog::getColor (QColor (133, 152, 181), this);
    if (c.isValid () == true) {
        cout << c.red () << endl;
        RayTracer::getInstance ()->setBackgroundColor (Vec3Df (c.red (), c.green (), c.blue ()));
        viewer->setBackgroundColor (c);
        viewer->updateGL ();
    }
}

void Window::showRayImage () {
    viewer->setDisplayMode (GLViewer::RayDisplayMode);
}

void Window::exportGLImage () {
    viewer->saveSnapshot (false, false);
}

void Window::exportRayImage () {
    QString filename = QFileDialog::getSaveFileName (this,
                                                     "Save ray-traced image",
                                                     ".",
                                                     "*.jpg *.bmp *.png");
    if (!filename.isNull () && !filename.isEmpty ())
        viewer->getRayImage().save (filename);
}

void Window::about () {
    QMessageBox::about (this, 
                        "About This Program", 
                        "<b>RayMini</b> <br> by <i>Tamy Boubekeur</i>.");
}

void Window::setShadowOption(int option) {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
  switch(option) {
  case 0:
    rayTracerInstance->setShadowOption(RAYTRACER_NO_SHADOW);
    break;
  case 1:
    rayTracerInstance->setShadowOption(RAYTRACER_HARD_SHADOW);
    break;
  case 2:
    rayTracerInstance->setShadowOption(RAYTRACER_SOFT_SHADOW);
    break;
  }
}

void Window::setAaOption(int option) {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
  switch(option) {
  case 0:
    rayTracerInstance->setAaOption(RAYTRACER_NO_AA);
    break;
  case 1:
    rayTracerInstance->setAaOption(RAYTRACER_AAx2);
    break;
  case 2:
    rayTracerInstance->setAaOption(RAYTRACER_AAx3);
    break;
  }
}

void Window::setAOOption(bool option) {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    rayTracerInstance->setAOOption(option);
}

int Window::getShadowNumberRay() {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    return rayTracerInstance->getShadowNbRay();
}
void Window::setShadowNumberRay(int NbRay) {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    rayTracerInstance->setShadowNbRay(NbRay);
}

double Window::getShadowRadius() {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    return rayTracerInstance->getShadowRadius();
}
void Window::setShadowRadius(double shadowRadius) {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    rayTracerInstance->setShadowRadius(shadowRadius);
}

int Window::getAONumberRay() {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    return rayTracerInstance->getAONbRay();
}
void Window::setAONumberRay(int NbRay) {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    rayTracerInstance->setAONbRay(NbRay);
}

int Window::getAOSphereRadius() {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    return rayTracerInstance->getAOSphereRadius();
}
void Window::setAOSphereRadius(int sphereRadius) {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    rayTracerInstance->setAOSphereRadius(sphereRadius);
}

int Window::getAOConeAngle() {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    return rayTracerInstance->getAOConeAngle();
}
void Window::setAOConeAngle(int coneAngle) {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    rayTracerInstance->setAOConeAngle(coneAngle);
}

double Window::getAOCoeff() {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    return rayTracerInstance->getAOCoeff();
}
void Window::setAOCoeff(double coeff) {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    rayTracerInstance->setAOCoeff(coeff);
}

void Window::initControlWidget () {
	scrollArea = new QScrollArea();
    controlWidget = new QGroupBox ();
    QVBoxLayout * layout = new QVBoxLayout (controlWidget);
    
    QGroupBox * previewGroupBox = new QGroupBox ("Preview", controlWidget);
    QVBoxLayout * previewLayout = new QVBoxLayout (previewGroupBox);
    
    QCheckBox * wireframeCheckBox = new QCheckBox ("Wireframe", previewGroupBox);
    connect (wireframeCheckBox, SIGNAL (toggled (bool)), viewer, SLOT (setWireframe (bool)));
    previewLayout->addWidget (wireframeCheckBox);
   
    QButtonGroup * modeButtonGroup = new QButtonGroup (previewGroupBox);
    modeButtonGroup->setExclusive (true);
    QRadioButton * flatButton = new QRadioButton ("Flat", previewGroupBox);
    QRadioButton * smoothButton = new QRadioButton ("Smooth", previewGroupBox);
    modeButtonGroup->addButton (flatButton, static_cast<int>(GLViewer::Flat));
    modeButtonGroup->addButton (smoothButton, static_cast<int>(GLViewer::Smooth));
    connect (modeButtonGroup, SIGNAL (buttonClicked (int)), viewer, SLOT (setRenderingMode (int)));
    previewLayout->addWidget (flatButton);
    previewLayout->addWidget (smoothButton);
    
    QPushButton * snapshotButton  = new QPushButton ("Save preview", previewGroupBox);
    connect (snapshotButton, SIGNAL (clicked ()) , this, SLOT (exportGLImage ()));
    previewLayout->addWidget (snapshotButton);

    layout->addWidget (previewGroupBox);
    
    QGroupBox * rayGroupBox = new QGroupBox ("Ray Tracing", controlWidget);
    QVBoxLayout * rayLayout = new QVBoxLayout (rayGroupBox);
    QPushButton * rayButton = new QPushButton ("Render", rayGroupBox);
    rayLayout->addWidget (rayButton);
    connect (rayButton, SIGNAL (clicked ()), this, SLOT (renderRayImage ()));
    QPushButton * showButton = new QPushButton ("Show", rayGroupBox);
    rayLayout->addWidget (showButton);
    connect (showButton, SIGNAL (clicked ()), this, SLOT (showRayImage ()));
    QPushButton * saveButton  = new QPushButton ("Save", rayGroupBox);
    connect (saveButton, SIGNAL (clicked ()) , this, SLOT (exportRayImage ()));
    rayLayout->addWidget (saveButton);

    // Anti aliasing options
    QComboBox *aaTypeList = new QComboBox(rayGroupBox);
    aaTypeList->addItem("No AA");
    aaTypeList->addItem("AA x 2");
    aaTypeList->addItem("AA x 3");
    connect (aaTypeList, SIGNAL (activated (int)), this, SLOT (setAaOption (int)));
    rayLayout->addWidget (aaTypeList);
    

    // Shadows options
    QComboBox *shadowTypeList = new QComboBox(rayGroupBox);
    shadowTypeList->addItem("No shadow");
    shadowTypeList->addItem("Hard shadow");
    shadowTypeList->addItem("Soft shadow");
    connect (shadowTypeList, SIGNAL (activated (int)), this, SLOT (setShadowOption (int)));
    rayLayout->addWidget (shadowTypeList);

	rayLayout->addWidget(new QLabel("Nb ray soft shadow:"));
	QSpinBox *shadowNumberRay = new QSpinBox();
	shadowNumberRay->setValue(getShadowNumberRay());
	shadowNumberRay->setRange(0, 1000000);
	connect(shadowNumberRay, SIGNAL(valueChanged(int)), this, SLOT (setShadowNumberRay(int)));
	rayLayout->addWidget(shadowNumberRay);

	rayLayout->addWidget(new QLabel("Radius SShadow L1:"));
	QDoubleSpinBox *shadowRadius = new QDoubleSpinBox();
	shadowRadius->setValue(getShadowRadius());
	shadowRadius->setRange(0, 100);
	shadowRadius->setSingleStep(0.01);
	connect(shadowRadius, SIGNAL(valueChanged(double)), this, SLOT (setShadowRadius(double)));
	rayLayout->addWidget(shadowRadius);
    
    layout->addWidget (rayGroupBox);

    // Ambient Occlusion Options
    QGroupBox * aoGroupBox = new QGroupBox ("Ambient Occlusion", controlWidget);
    QVBoxLayout * aoLayout = new QVBoxLayout (aoGroupBox);
    QCheckBox *AOCheckBox = new QCheckBox("Enable AO", aoGroupBox);
	connect (AOCheckBox, SIGNAL(toggled (bool)), this, SLOT(setAOOption(bool)));
	aoLayout->addWidget(AOCheckBox);

	aoLayout->addWidget(new QLabel("Number of ray:"));
	QSpinBox *AONumberIteration = new QSpinBox();
	AONumberIteration->setValue(getAONumberRay());
	AONumberIteration->setRange(0, 100000);
	connect(AONumberIteration, SIGNAL(valueChanged(int)), this, SLOT (setAONumberRay(int)));
	aoLayout->addWidget(AONumberIteration);

	aoLayout->addWidget(new QLabel("Sphere radius (%):"));
	QSpinBox *AOSphereRad = new QSpinBox();
	AOSphereRad->setValue(getAOSphereRadius());
	AOSphereRad->setRange(0, 100);
	connect(AOSphereRad, SIGNAL(valueChanged(int)), this, SLOT (setAOSphereRadius(int)));
	aoLayout->addWidget(AOSphereRad);

	aoLayout->addWidget(new QLabel("Cone Angle:"));
	QSpinBox *AOConeAng = new QSpinBox();
	AOConeAng->setValue(getAOConeAngle());
	AOConeAng->setRange(0, 90);
	connect(AOConeAng, SIGNAL(valueChanged(int)), this, SLOT (setAOConeAngle(int)));
	aoLayout->addWidget(AOConeAng);

	aoLayout->addWidget(new QLabel("AO Coeff:"));
	QDoubleSpinBox *AOCoeff = new QDoubleSpinBox();
	AOCoeff->setValue(getAOCoeff());
	AOCoeff->setRange(0.0, 1.0);
	AOCoeff->setSingleStep(0.01);
	connect(AOCoeff, SIGNAL(valueChanged(double)), this, SLOT (setAOCoeff(double)));
	aoLayout->addWidget(AOCoeff);

    layout->addWidget (aoGroupBox);

	// Objects properties
    QGroupBox * objectsGroupBox = new QGroupBox ("Objects Properties", controlWidget);
	QVBoxLayout * objectsLayout = new QVBoxLayout(objectsGroupBox);

	Scene *scene = Scene::getInstance();

	for (unsigned int i = 0; i < scene->getObjects().size(); i++)
		objectsLayout->addWidget(getWidgetObject(scene->getObjects()[i], objectsGroupBox));

	layout->addWidget(objectsGroupBox);

    
    QGroupBox * globalGroupBox = new QGroupBox ("Global Settings", controlWidget);
    QVBoxLayout * globalLayout = new QVBoxLayout (globalGroupBox);
    
    QPushButton * bgColorButton  = new QPushButton ("Background Color", globalGroupBox);
    connect (bgColorButton, SIGNAL (clicked()) , this, SLOT (setBGColor()));
    globalLayout->addWidget (bgColorButton);
    
    QPushButton * aboutButton  = new QPushButton ("About", globalGroupBox);
    connect (aboutButton, SIGNAL (clicked()) , this, SLOT (about()));
    globalLayout->addWidget (aboutButton);
    
    QPushButton * quitButton  = new QPushButton ("Quit", globalGroupBox);
    connect (quitButton, SIGNAL (clicked()) , qApp, SLOT (closeAllWindows()));
    globalLayout->addWidget (quitButton);

    layout->addWidget (globalGroupBox);

    layout->addStretch (0);
	scrollArea->setWidget(controlWidget);

}

QGroupBox * Window::getWidgetObject (Object &o, QWidget *parent) {
	QGroupBox *objectGroupBox = new QGroupBox(o.getName(), parent);
	QVBoxLayout *objectLayout = new QVBoxLayout(objectGroupBox);

	objectLayout->addWidget(new QLabel("Reflectance:"));

	QDoubleSpinBox *mirrorSpinBox = new QDoubleSpinBox();
	mirrorSpinBox->setValue(o.getMaterial().getReflectance());
	mirrorSpinBox->setRange(0.0, 10.0);
	mirrorSpinBox->setSingleStep(0.05);

	connect(mirrorSpinBox, SIGNAL(valueChanged(double)), new MaterialQObject(&o.getMaterial()),
			SLOT(setReflectance(double)));

	objectLayout->addWidget(mirrorSpinBox);
	return objectGroupBox;

}
