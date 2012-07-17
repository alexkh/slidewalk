#include <QtGui/QApplication>
#include "mainbox.hpp"

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	MainBox w;
	w.show();

	return a.exec();
}
