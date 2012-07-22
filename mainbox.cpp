#include "mainbox.hpp"
#include "ui_mainbox.h"

MainBox::MainBox(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainBox) {
	osr = NULL;
	imgbuf = NULL;
	ui->setupUi(this);

	if(1) {
		statusBar()->showMessage(tr("Ready"));
	} else {
		statusBar()->showMessage(tr("Error"));
	}
}

MainBox::~MainBox() {
	delete ui;
	if(imgbuf != NULL) {
		delete [] imgbuf;
	}
}

void MainBox::on_actionFileOpen_triggered() {
	statusBar()->showMessage(tr("Open File dialog activated"));
	slideFileName = QFileDialog::getOpenFileName(this,
			tr("Open File"), ".",
			tr("OpenSlide Files (*)"));
	if(slideFileName.isEmpty()) {
		statusBar()->showMessage(tr("Opening Cancelled..."), 2000);
		return;
	}
	QByteArray byteArray = slideFileName.toUtf8();
	const char *fname = byteArray.constData();
	if(openslide_can_open(fname)) {
		statusBar()->showMessage(tr("Trying to open ") +
							slideFileName);
	} else {
		statusBar()->showMessage(tr("Can't open file ") +
							slideFileName);
		return;
	}
	// now we must free the osr object if it's not NULL
	if(osr != NULL) {
		openslide_close(osr);
	}
	// open file
	osr = openslide_open(fname);
	if(osr == NULL) {
		statusBar()->showMessage(tr("Error during opening file ") +
							slideFileName);
	}

	// we get the count of levels, and then grab the image from the topmost
	// level
	int32_t level_count;
//	level_count = openslide_get_level_count(osr); // future interface
	level_count = openslide_get_layer_count(osr); // backward compatibility


	statusBar()->showMessage(tr("Level count ") +
		QString::number(level_count));

	// get top level's dimensions
	int64_t w, h;
//	openslide_get_level_dimensions(osr, level_count - 1, &w, &h); // future
	openslide_get_layer_dimensions(osr, level_count - 1, &w, &h);

	statusBar()->showMessage(tr("Top level dimensions: ") +
		QString::number(w) + " by " + QString::number(h));

	// now if dimensions are less than 3000x3000 we grab the whole img
	w = w > 3000? 3000: w;
	h = h > 3000? 3000: h;
	imgbuf = new uint32_t[w * h];
	openslide_read_region(osr, imgbuf, 0, 0, level_count - 1, w, h);

	QImage img((uchar *)imgbuf, w, h, QImage::Format_ARGB32);
	ui->label->setPixmap(QPixmap::fromImage(img));
/*	if(!img.loadFromData((const uchar *)imgbuf, w * h * 4)) {
		statusBar()->showMessage(tr("Error loading image data"));
	}
*/


}
