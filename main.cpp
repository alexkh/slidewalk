#include <csignal>
#include <QtGui/QApplication>
#include <QTextCodec>
#include "mainbox.hpp"

struct CleanExit{
	CleanExit() {
		signal(SIGINT, &CleanExit::exitQt);
		signal(SIGTERM, &CleanExit::exitQt);
	}

	static void exitQt(int sig) {
		QCoreApplication::exit(0);
	}
};

int main(int argc, char *argv[]) {
	QCoreApplication::setOrganizationName("SlideWalk Project");
	QCoreApplication::setApplicationName("SlideWalk");
	QCoreApplication::setApplicationVersion("0.1");
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

	CleanExit cleanExit;
	QApplication a(argc, argv);
	MainBox w;
	w.show();

	return a.exec();
}
