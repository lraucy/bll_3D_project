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
#include "LightQObject.h"

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
    initMenu ();
    
    controlDockWidget->setWidget (controlWidget);
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
	rayTracer->setTracing(modeTracing);
    viewer->setRayImage(rayTracer->render (viewer->getRayImage(), camPos, viewDirection, upVector, rightVector,
					   fieldOfView, aspectRatio, screenWidth, screenHeight));
    statusBar()->showMessage(QString ("Raytracing performed in ") +
                             QString::number (timer.elapsed ()) +
                             QString ("ms at ") +
                             QString::number (screenWidth) + QString ("x") + QString::number (screenHeight) +
                             QString (" screen resolution"));
    viewer->setDisplayMode (GLViewer::RayDisplayMode);
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

void Window::setScene(int option) {
  Scene * sceneInstance = Scene::getInstance ();
  switch(option) {
  case 0:
    sceneInstance->switchScene(SCENE_DEFAULT_SCENE);
    break;
  case 1:
    sceneInstance->switchScene(SCENE_CLOSED_BOX_SCENE);
    break;
  case 2:
    sceneInstance->switchScene(SCENE_FINAL_SCENE);
    break;
  case 3:
    sceneInstance->switchScene(SCENE_DOUBLE_LIGHT_SCENE);
    break;
  case 4:
    sceneInstance->switchScene(SCENE_CLOSED_BOX_PATH_TRACING_SCENE);
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

void Window::setRayOption(int option) {
  switch(option) {
  case 0:
    modeTracing = RAY_TRACING;
    break;
  case 1:
    modeTracing = PATH_TRACING;
    break;
  case 2:
	modeTracing = PATH_TRACING_LOIC;
	break;
  }
}

void Window::setAOOption(bool option) {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    rayTracerInstance->setAOOption(option);
}

void Window::setBrdfOption(bool option) {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    rayTracerInstance->setBrdfOption(option);
}

int Window::getShadowNumberRay() {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    return rayTracerInstance->getShadowNbRay();
}
void Window::setShadowNumberRay(int NbRay) {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
  rayTracerInstance->setShadowNbRay((unsigned int)NbRay);
}

double Window::getShadowRadius() {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    return rayTracerInstance->getShadowRadius();
}
void Window::setShadowRadius(double shadowRadius) {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    rayTracerInstance->setShadowRadius(shadowRadius);
}

int Window::getPathTraceDepthLoic() {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
  return rayTracerInstance->getPathTraceDepthLoic();
}

void Window::setPathTraceDepthLoic(int pathTraceDepth) {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
  rayTracerInstance->setPathTraceDepthLoic(pathTraceDepth);
}

int Window::getIterationPerTracingLoic() {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
  return rayTracerInstance->getIterationPerTracingLoic();
}

void Window::setIterationPerTracingLoic(int iteration) {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
  rayTracerInstance->setIterationPerTracingLoic(iteration);
}

void Window::resetPathTracingLoic() {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
  rayTracerInstance->resetPathTracingLoic();
}

void Window::setMaxDepth(int depth) {
	KdTreeElement::setMaxDepth(depth);
}
void Window::setTrianglePerLeaf(int triangleNb) {
	KdTreeElement::setTrianglePerLeaf(triangleNb);
}


int Window::getPathTraceNumberRay() {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    return rayTracerInstance->getPathTraceNbRay();
}
void Window::setPathTraceNumberRay(int NbRay) {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    rayTracerInstance->setPathTraceNbRay(NbRay);
}

double Window::getPathTraceDepth() {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    return rayTracerInstance->getPathTraceDepth();
}
void Window::setPathTraceDepth(int depth) {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    rayTracerInstance->setPathTraceDepth(depth);
}

double Window::getPathTraceAngle() {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    return rayTracerInstance->getPathTraceAngle();
}
void Window::setPathTraceAngle(double angle) {
  RayTracer * rayTracerInstance = RayTracer::getInstance ();
    rayTracerInstance->setPathTraceAngle(angle);
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

void Window::rebuildKdTree() {
	Scene * scene = Scene::getInstance();
	scene->rebuildKdTree();
}

void Window::initMenu() {
	controlWidget = new QGroupBox();
	QVBoxLayout *layout = new QVBoxLayout(controlWidget);

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

    // RayTracing
    QComboBox *rayTypeList = new QComboBox(rayGroupBox);
    rayTypeList->addItem("Simple Ray Tracing");
    rayTypeList->addItem("Laert's Path Tracing");
    rayTypeList->addItem("Loic's Path Tracing");
    connect (rayTypeList, SIGNAL (activated (int)), this, SLOT (setRayOption (int)));
    rayLayout->addWidget (rayTypeList);

    layout->addWidget (rayGroupBox);

	QComboBox *sceneList = new QComboBox(controlWidget);
	sceneList->addItem("Default scene");
	sceneList->addItem("Closed box scene");
	sceneList->addItem("Final scene");
	sceneList->addItem("Double light scene");
	sceneList->addItem("Path Tracing scene");
    connect (sceneList, SIGNAL (activated (int)), this, SLOT (setScene (int)));
    layout->addWidget (sceneList);


	QTabWidget *qTabWidget = new QTabWidget();

	qTabWidget->addTab(getGeneralTabWidget(), "General");
	qTabWidget->addTab(getGeneralSettingsTabWidget(), "Simple Ray Tracing");
	qTabWidget->addTab(getPathTracingSettingsTabWidget(), "PathTracing");
	qTabWidget->addTab(getKdTreeSettingsTabWidget(), "KdTree");
	qTabWidget->addTab(getSceneSettingsTabWidget(), "Scene");

	layout->addWidget(qTabWidget);
}

QWidget * Window::getGeneralTabWidget() {
    QGroupBox *generalWidget = new QGroupBox ();
    QVBoxLayout * layout = new QVBoxLayout (generalWidget);

    QGroupBox * previewGroupBox = new QGroupBox ("Preview", generalWidget);
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


    QGroupBox * globalGroupBox = new QGroupBox ("Global Settings", generalWidget);
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

	return generalWidget;

}

QWidget * Window::getGeneralSettingsTabWidget() {
    QGroupBox *generalWidget = new QGroupBox ();
    QVBoxLayout * layout = new QVBoxLayout (generalWidget);

    QCheckBox *brdfCheckBox = new QCheckBox("Enable direct illumination", generalWidget);
	brdfCheckBox->setChecked(true);
	connect (brdfCheckBox, SIGNAL(toggled (bool)), this, SLOT(setBrdfOption(bool)));
	layout->addWidget(brdfCheckBox);

    QComboBox *aaTypeList = new QComboBox(generalWidget);
    aaTypeList->addItem("No AA");
    aaTypeList->addItem("AA x 2");
    aaTypeList->addItem("AA x 3");
    connect (aaTypeList, SIGNAL (activated (int)), this, SLOT (setAaOption (int)));
    layout->addWidget (aaTypeList);
    
    // Shadows options
    QComboBox *shadowTypeList = new QComboBox(generalWidget);
    shadowTypeList->addItem("No shadow");
    shadowTypeList->addItem("Hard shadow");
    shadowTypeList->addItem("Soft shadow");
    connect (shadowTypeList, SIGNAL (activated (int)), this, SLOT (setShadowOption (int)));
    layout->addWidget (shadowTypeList);

	layout->addWidget(new QLabel("Nb ray soft shadow:"));
	QSpinBox *shadowNumberRay = new QSpinBox();
	shadowNumberRay->setValue(getShadowNumberRay());
	shadowNumberRay->setRange(0, 1000000);
	connect(shadowNumberRay, SIGNAL(valueChanged(int)), this, SLOT (setShadowNumberRay(int)));
	layout->addWidget(shadowNumberRay);

	layout->addWidget(new QLabel("Radius SShadow L1:"));
	QDoubleSpinBox *shadowRadius = new QDoubleSpinBox();
	shadowRadius->setValue(getShadowRadius());
	shadowRadius->setRange(0, 100);
	shadowRadius->setSingleStep(0.01);
	connect(shadowRadius, SIGNAL(valueChanged(double)), this, SLOT (setShadowRadius(double)));
	layout->addWidget(shadowRadius);

    // Ambient Occlusion Options
    QGroupBox * aoGroupBox = new QGroupBox ("Ambient Occlusion", generalWidget);
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
    
	return generalWidget;
}

QWidget * Window::getPathTracingSettingsTabWidget() {
    QGroupBox *pathTraceWidget = new QGroupBox ();
    QVBoxLayout * layout = new QVBoxLayout (pathTraceWidget);

    QGroupBox * ptGroupBox = new QGroupBox ("Laert's Path tracing", pathTraceWidget);
    QVBoxLayout * ptLayout = new QVBoxLayout (ptGroupBox);
    ptLayout->addWidget(new QLabel("Nb ray pathTrace:"));
    QSpinBox *pathTraceNumberRay = new QSpinBox();
    pathTraceNumberRay->setValue(getPathTraceNumberRay());
    pathTraceNumberRay->setRange(0, 1000000);
    connect(pathTraceNumberRay, SIGNAL(valueChanged(int)), this, SLOT (setPathTraceNumberRay(int)));
    ptLayout->addWidget(pathTraceNumberRay);

    ptLayout->addWidget(new QLabel("Angle (only Laert's):"));
    QDoubleSpinBox *pathTraceAngle = new QDoubleSpinBox();
    pathTraceAngle->setValue(getPathTraceAngle());
    pathTraceAngle->setRange(0, M_PI);
    pathTraceAngle->setSingleStep(0.01);
    connect(pathTraceAngle, SIGNAL(valueChanged(double)), this, SLOT (setPathTraceAngle(double)));
    ptLayout->addWidget(pathTraceAngle);
    
    ptLayout->addWidget(new QLabel("Depth:"));
    QSpinBox *pathTraceDepth = new QSpinBox();
    pathTraceDepth->setValue(getPathTraceDepth());
    pathTraceDepth->setRange(0, 1000000);
    pathTraceDepth->setSingleStep(1);
    connect(pathTraceDepth, SIGNAL(valueChanged(int)), this, SLOT (setPathTraceDepth(int)));
    ptLayout->addWidget(pathTraceDepth);

	layout->addWidget(ptGroupBox);

	// Loic Path tracing options

    QGroupBox * ptLoicGroupBox = new QGroupBox ("Loic's Path tracing", pathTraceWidget);
    QVBoxLayout * ptLoicLayout = new QVBoxLayout (ptLoicGroupBox);


    ptLoicLayout->addWidget(new QLabel("Iteration:"));
    QSpinBox *pathLoicTraceIteration = new QSpinBox();
    pathLoicTraceIteration->setValue(getIterationPerTracingLoic());
    pathLoicTraceIteration->setRange(0, 1000000);
    pathLoicTraceIteration->setSingleStep(1);
    connect(pathLoicTraceIteration, SIGNAL(valueChanged(int)), this, SLOT (setIterationPerTracingLoic(int)));
    ptLoicLayout->addWidget(pathLoicTraceIteration);

    ptLoicLayout->addWidget(new QLabel("Depth:"));
    QSpinBox *pathLoicTraceDepth = new QSpinBox();
    pathLoicTraceDepth->setValue(getPathTraceDepthLoic());
    pathLoicTraceDepth->setRange(0, 1000000);
    pathLoicTraceDepth->setSingleStep(1);
    connect(pathLoicTraceDepth, SIGNAL(valueChanged(int)), this, SLOT (setPathTraceDepthLoic(int)));
    ptLoicLayout->addWidget(pathLoicTraceDepth);

    layout->addWidget (ptLoicGroupBox);

	return pathTraceWidget;
}

QWidget * Window::getKdTreeSettingsTabWidget() {
    QGroupBox * kdGroupBox = new QGroupBox ("KdTree", controlWidget);
    QVBoxLayout * kdLayout = new QVBoxLayout (kdGroupBox);

	kdLayout->addWidget(new QLabel("Depth"));
	QSpinBox *kdDepth = new QSpinBox();
	kdDepth->setValue(KdTreeElement::maxDepth);
	kdDepth->setRange(0, 100000);
	connect(kdDepth, SIGNAL(valueChanged(int)), this, SLOT (setMaxDepth(int)));
	kdLayout->addWidget(kdDepth);

	kdLayout->addWidget(new QLabel("Triangle nb"));
	QSpinBox *kdTriangle = new QSpinBox();
	kdTriangle->setValue(KdTreeElement::trianglePerLeaf);
	kdTriangle->setRange(0, 100000);
	connect(kdTriangle, SIGNAL(valueChanged(int)), this, SLOT (setTrianglePerLeaf(int)));
	kdLayout->addWidget(kdTriangle);

    QPushButton * rebuildKdButton = new QPushButton ("rebuild Kd", kdGroupBox);
    kdLayout->addWidget (rebuildKdButton);
    connect (rebuildKdButton, SIGNAL (clicked ()), this, SLOT (rebuildKdTree ()));

	return kdGroupBox;

}


QWidget * Window::getSceneSettingsTabWidget() {
	Scene *scene = Scene::getInstance();
	QScrollArea *qScrollArea = new QScrollArea();
    QGroupBox *sceneWidget = new QGroupBox ();
    QVBoxLayout * layout = new QVBoxLayout (sceneWidget);

	// Objects properties
    QGroupBox * objectsGroupBox = new QGroupBox ("Objects Properties", controlWidget);
	QVBoxLayout * objectsLayout = new QVBoxLayout(objectsGroupBox);

	for (unsigned int i = 0; i < scene->getObjects().size(); i++)
		objectsLayout->addWidget(getWidgetObject(scene->getObjects()[i], objectsGroupBox));

	layout->addWidget(objectsGroupBox);

	// Lights properties
    QGroupBox * lightsGroupBox = new QGroupBox ("Lights Properties", controlWidget);
	QVBoxLayout * lightsLayout = new QVBoxLayout(lightsGroupBox);

	for (unsigned int i = 0; i < scene->getLights().size(); i++)
		lightsLayout->addWidget(getWidgetLight(scene->getLights()[i], lightsGroupBox));

	layout->addWidget(lightsGroupBox);

	qScrollArea->setWidget(sceneWidget);
	return qScrollArea;
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

QGroupBox * Window::getWidgetLight (Light &l, QWidget *parent) {
	QGroupBox *lightGroupBox = new QGroupBox(l.getName(), parent);
	QVBoxLayout *lightLayout = new QVBoxLayout(lightGroupBox);

	// Intensity
	lightLayout->addWidget(new QLabel("Intensity:"));

	QDoubleSpinBox *mirrorSpinBox = new QDoubleSpinBox();
	mirrorSpinBox->setValue(l.getIntensity());
	mirrorSpinBox->setRange(0.0, 20.0);
	mirrorSpinBox->setSingleStep(0.05);
	
	connect(mirrorSpinBox, SIGNAL(valueChanged(double)), new LightQObject(&l),
			SLOT(setIntensity(double)));
	
	lightLayout->addWidget(mirrorSpinBox);

	// Color
	lightLayout->addWidget(new QLabel("Color (RGB values):"));

	QDoubleSpinBox *redMirrorSpinBox = new QDoubleSpinBox();
	QDoubleSpinBox *greenMirrorSpinBox = new QDoubleSpinBox();
	QDoubleSpinBox *blueMirrorSpinBox = new QDoubleSpinBox();
	redMirrorSpinBox->setValue(l.getColor()[0]);
	redMirrorSpinBox->setRange(0.0, 1.0);
	redMirrorSpinBox->setSingleStep(0.05);
	
	greenMirrorSpinBox->setValue(l.getColor()[1]);
	greenMirrorSpinBox->setRange(0.0, 1.0);
	greenMirrorSpinBox->setSingleStep(0.05);
	
	blueMirrorSpinBox->setValue(l.getColor()[2]);
	blueMirrorSpinBox->setRange(0.0, 1.0);
	blueMirrorSpinBox->setSingleStep(0.05);
	
	connect(redMirrorSpinBox, SIGNAL(valueChanged(double)), new LightQObject(&l),
			SLOT(setRed(double)));
	connect(greenMirrorSpinBox, SIGNAL(valueChanged(double)), new LightQObject(&l),
			SLOT(setGreen(double)));
	connect(blueMirrorSpinBox, SIGNAL(valueChanged(double)), new LightQObject(&l),
			SLOT(setBlue(double)));
	
	lightLayout->addWidget(redMirrorSpinBox);
	lightLayout->addWidget(greenMirrorSpinBox);
	lightLayout->addWidget(blueMirrorSpinBox);
	
	return lightGroupBox;

}
