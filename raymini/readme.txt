RayMini v0.2
Author: Tamy Boubekeur (tamy.boubekeur@telecom-paristech.fr)
RayMini is a minimal raytracer implemented in C++/OpenGL/Qt.
This software package is meants to be distributed to Telecom ParisTech student only.

Compiling
----------
Required library: GCC >v4, OpenGL, QT >v4.4, libQGLViewer and GLEW (for GPU extensions)
Edit the file raymini.pro to adapt it to your configuration (has been tested under Linux Ubuntu and Win7).

On linux: 
 qmake raymini.pro
 make
 ./raymini

On windows:
 Use QT Creator and do not forget to setup the run configuration properly so that the raymini directory is the working directory. 
 Otherwise, the 'models' directory will not be found by the raymini executable.
 If you want to run the executable directly (by double-clikcing on it in the Windows file explorer), copy the model directory into the release directory first 
 and double-click on raymini.exe. 