#ifndef WINDOW_H
#define WINDOW_H

#include "GLViewer.h"
#include <QMainWindow>
#include <QAction>
#include <QToolBar>
#include <QScrollArea>
#include <QCheckBox>
#include <QGroupBox>
#include <QComboBox>
#include <QSlider>
#include <QLCDNumber>
#include <QSpinBox>
#include <QImage>
#include <QLabel>

#include <vector>
#include <string>

#include "QTUtils.h"


class Window : public QMainWindow {
    Q_OBJECT
public:
    Window();
    virtual ~Window();

    static void showStatusMessage (const QString & msg);  
    
public slots :
    void renderRayImage ();
    void setBGColor ();
    void showRayImage ();
    void exportGLImage ();
    void exportRayImage ();
    void about ();

	void setRayOption(int option);
  void setShadowOption(int option);
  void setAaOption(int option);
  void setScene(int option);
  void setShadowNumberRay(int nbRay);
  int getShadowNumberRay();
  void setShadowRadius(double shadowRadius);
  double getShadowRadius();
  int getPathTraceDepthLoic();
  void setPathTraceDepthLoic(int pathTraceDepth);
  int getIterationPerTracingLoic();
  void setIterationPerTracingLoic(int iteration);
  void resetPathTracingLoic();
  void setAOOption(bool option);
  void setAONumberRay(int nbRay);
  int getAONumberRay();
  void setAOSphereRadius(int sphereRadius);
  int getAOSphereRadius();
  void setAOConeAngle(int coneAngle);
  int getAOConeAngle();
  void setAOCoeff(double coeff);
  double getAOCoeff();
  int getPathTraceNumberRay() ;
  void setPathTraceNumberRay(int NbRay) ;
  double getPathTraceDepth() ;
  void setPathTraceDepth(int depth) ;
  double getPathTraceAngle() ;
  void setPathTraceAngle(double angle) ;
  void setMaxDepth(int depth);
  void setTrianglePerLeaf(int triangleNb);
  void rebuildKdTree();

private :
    void initMenu ();
	QWidget * getGeneralTabWidget();
	QWidget * getGeneralSettingsTabWidget();
	QWidget * getPathTracingSettingsTabWidget();
	QWidget * getKdTreeSettingsTabWidget();
	QWidget * getSceneSettingsTabWidget();
	QGroupBox * getWidgetObject (Object &o, QWidget *parent);
	QGroupBox * getWidgetLight (Light &l, QWidget *parent);
        
    QActionGroup * actionGroup;
	QScrollArea * scrollArea;
	QWidget * controlWidget;
    QString currentDirectory;
	int modeTracing;

    GLViewer * viewer;
};

#endif // WINDOW_H


// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
