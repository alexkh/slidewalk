slidewalk
=========

A little QT app to test Openslide library (see openslide.org)

License: GPL

This application depends heavily on Openslide, which has to be installed first.
openslide.h needs to be in your include path for compiling, and
libopenslide-0.dll for Windows or libopenslide.so for Linux
must be in lib path for linking.

In Windows, after build, these .dll files will need to be put in the same
dir as the slidewalk.exe before running it:

QtCore4.dll
QtGui4.dll
freetype6.dll
intl.dll
libcairo-2.dll
libexpat-1.dll
libfontconfig-1.dll
libgcc_s_dw2-1.dll
libglib-2.0-0.dll
libgthread-2.0-0.dll
libjpeg-8.dll
libopenjpeg-1.dll
libopenslide-0.dll
libpng14-14.dll
libtiff-3.dll
libxml2-2.dll
mingwm10.dll
msvcr80.dll
zlib.dll

To build in Linux:
qmake
make

To build in Windows (using QtCreator):
- launch the QtCreator by double-clicking the slidewalk.pro
- build the project
- move all necessary dll's to where slidewalk.exe is

In this first release this app does not do much: it lets you choose the file,
opens it, and displays top level image cropped to 3000x3000 pixels if needed.

