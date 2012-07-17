#ifndef MAINBOX_HPP
#define MAINBOX_HPP

#include <stdint.h>
#include <QMainWindow>
#include <QFileDialog>
#include <QStatusBar>
#include <QImage>
extern "C" {
	#include "openslide.h"
}

namespace Ui {
	class MainBox;
}

class MainBox : public QMainWindow {
Q_OBJECT

public:
	explicit MainBox(QWidget *parent = 0);
	~MainBox();

private slots:
	void on_actionFileOpen_triggered();

private:
	Ui::MainBox *ui;
	QString slideFileName;
	openslide_t *osr;
	uint32_t *imgbuf;
};

#endif // MAINBOX_HPP
