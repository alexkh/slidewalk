#include "mainbox.hpp"
#include "ui_mainbox.h"
#include <qmath.h>
#include <string.h>

MainBox::MainBox(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainBox) {
	osr = NULL;
	imgbuf = NULL;
	ui->setupUi(this);
	scene = NULL;
	tile_loaded = NULL;
	QSettings::setDefaultFormat(QSettings::IniFormat);
	readSettings();

	if(1) {
		statusBar()->showMessage(tr("Ready"));
	} else {
		statusBar()->showMessage(tr("Error"));
	}
	view = new View("Main View", this);
	ui->mainLayout->addWidget(view);
	connect(view->zoomSlider, SIGNAL(valueChanged(int)),
					this, SLOT(on_zoom_changed(int)));

	// initialize all the tile data
; // this value x this value pixels for each tile
	total_levels = 0; // nothing loaded yet
	level = NULL;

}

MainBox::~MainBox() {
	delete ui;
	if(imgbuf != NULL) {
		delete [] imgbuf;
	}
	if(level != NULL) {
		delete [] level;
	}
	if(tile_loaded != NULL) {
		delete [] tile_loaded;
	}
}

void MainBox::closeEvent(QCloseEvent *event) {
	writeSettings();
	event->accept();
}

void MainBox::writeSettings() {
	QSettings settings;
	settings.setIniCodec("UTF-8");

	settings.beginGroup("MainBox");
	settings.setValue("size", size());
	settings.setValue("pos", pos());
	settings.setValue("dir", curDir);
	settings.endGroup();
	settings.sync();
}

void MainBox::writeSetting(const QString &key,
				const QVariant &value, bool sync) {
	QSettings settings;
	settings.setIniCodec("UTF-8");
	settings.setValue(key, value);
	if(sync) {
		settings.sync();
	}
}

void MainBox::readSettings() {
	QSettings settings;
	settings.setIniCodec("UTF-8");

	settings.beginGroup("MainBox");
	resize(settings.value("size", QSize(400, 400)).toSize());
	move(settings.value("pos", QPoint(200, 200)).toPoint());
	curDir = settings.value("dir", ".").toString();
	settings.endGroup();
}

void MainBox::on_actionFileOpen_triggered() {
	statusBar()->showMessage(tr("Open File dialog activated"));
	slideFileName = QFileDialog::getOpenFileName(this,
			tr("Open File"), curDir,
			tr("OpenSlide Files (*)"));
	if(slideFileName.isEmpty()) {
		statusBar()->showMessage(tr("Opening Cancelled..."), 2000);
		return;
	}
	// update curDir, by extracting dir name from file path
	QFileInfo fileInfo(slideFileName);
	curDir = fileInfo.absolutePath();
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

	total_levels = level_count;
	qDebug() << "Total levels: " << total_levels;
	if(total_levels < 1) {
		return;
	}

	// allocate enough space for level_start_tile_id
	if(level != NULL) {
		delete [] level;
	}
	level = new LevelData[total_levels];
	int i;
	uint64_t cur_tile = 0;
	for(i = 0; i < total_levels; i++) {
//		openslide_get_level_dimensions(osr, i,
//				&(level[i].w), &(level[i].h)); // future
		openslide_get_layer_dimensions(osr, i, // backward
				&(level[i].w), &(level[i].h)); // compatibility
		// we must also calculate max_zoom for each level
		level[i].scale = qreal(level[i].w) / qreal(level[0].w);
		level[i].max_zoom =
			int(((qLn(level[i].scale) / qLn(2)) * 50) + 500.0);
		qDebug() << "level " << i << " max zoom " << level[i].max_zoom;
		// now we calculate how many tiles this image requires
		cur_tile = level[i].recalc(cur_tile);
	}
	if(tile_loaded != NULL) {
		delete [] tile_loaded;
	}
	total_tiles = cur_tile;
	tile_loaded = new bool[total_tiles];
	memset(tile_loaded, false, total_tiles);

	// we set the scene size to the size of level0 image
	if(scene != NULL) {
		delete scene;
	}
	scene = new QGraphicsScene(this);
	view->view()->setScene(scene);
	view->view()->setSceneRect(0, 0, level[0].w, level[0].h);
	view->view()->SetCenter(QPointF(level[0].w / 2.0,
						level[0].h / 2.0));

	// grab tiles of the topmost image
	int j;
	QImage *img;
	QGraphicsPixmapItem *item;
	int ts = LevelData::tile_side;
	int w = ts, h = ts; // used for partial tile sizes
	qreal scale = level[0].w / level[total_levels - 1].w;
	int cur_l = total_levels - 1;
	LevelData &cur_level = level[cur_l];
	imgbuf = new uint32_t[ts * ts];
	for(i = 0; i < cur_level.w_tiles; i++) {
		for(j = 0; j < cur_level.h_tiles; j++) {
			if(i == cur_level.w_tiles - 1) {
				w = (cur_level.w - i * ts) - 1;
			} else {
				w = ts;
			}
			if(j == cur_level.h_tiles - 1) {
				h = (cur_level.h - j * ts) - 1;
			} else {
				h = ts;
			}
			qDebug() << i * ts << " " << j * ts << " : " << w
				<< " " << h;
			openslide_read_region(osr, imgbuf,
				i * ts * scale, j * ts * scale, cur_l, w, h);
			img = new QImage((uchar *)imgbuf, w, h,
					QImage::Format_ARGB32);
			item = scene->addPixmap(QPixmap::fromImage(*img));
			delete img;
			item->setScale(scale);
			item->setPos(i * ts * scale, j * ts * scale);
		}
	}
	scene->clear();

	// set center and scale to fit image in the view
	// we need to know the size of the view and compare it to size of img
	qreal scale1 = qreal(view->view()->width()) / qreal(level[0].w);
	qreal scale2 = qreal(view->view()->height()) / qreal(level[0].h);
	scale = scale1 < scale2? scale1: scale2;
	qreal r, r1; // temporary variable to calculate val
	r = (((qLn(scale) / qLn(2)) * 50) + 500.0);
//	r1 = qPow(qreal(2), qreal(r - 500) / qreal(50));
//	qDebug() << scale1 << " " << scale2 << " - " << r << ", " << r1;
	view->zoomSlider->setValue(int(r));

//	view->view()->scale(scale, scale);
//	view->zoomSlider->setValue(0);
//	view->setupMatrix();
//	view->view()->zoomChanged(scale * view->view()->transform().m11());

/*
	// get top level's dimensions
	statusBar()->showMessage(tr("Top level dimensions: ") +
QString::number(w) + " by " + QString::number(h));

	// now if dimensions are less than 3000x3000 we grab the whole img
	w = w > 3000? 3000: w; h = h > 3000? 3000: h; imgbuf = new uint32_t[w *
h]; openslide_read_region(osr, imgbuf, 0, 0, level_count - 1, w, h);

	QImage img((uchar *)imgbuf, w, h, QImage::Format_ARGB32);
*/
//	ui->label->setPixmap(QPixmap::fromImage(img));
/*	if(!img.loadFromData((const uchar *)imgbuf, w * h * 4)) {
		statusBar()->showMessage(tr("Error loading image data")); }
*/


}

void MainBox::on_zoom_changed(int zoom) {
	if(!total_levels) {
		return;
	}
	// determine which level we are on now
	int i, cur_l = 0;
	for(i = total_levels; i >= 0; i--) {
		cur_l = i;
		if(zoom < level[i].max_zoom) {
			break;
		}
	}
	// determine which tiles are currently visible
	// to do that, we must first convert view coordinates to scene coords,
	// and then scene coords to level coords to get bounding box.
	// after that we'll iterate through each tile in that bounding box...
	QRectF bounds =
		view->view()->mapToScene(view->view()->rect()).boundingRect();
	// now we can map these scene coordinates to level coordinates by
	// scaling them down...
	qreal ts = LevelData::tile_side;
	qreal top = bounds.top() * level[i].scale / ts;
	qreal left = bounds.left() * level[i].scale / ts;
	qreal right = bounds.right() * level[i].scale / ts;
	qreal bottom = bounds.bottom() * level[i].scale / ts;
	QRectF tile_bounds(left, top, right - left, bottom - top);
	QRectF trimmed = tile_bounds.intersected(QRectF(0, 0, level[i].w_tiles,
						level[i].h_tiles));
	int j;
	uint64_t cur_tile;
	left = qFloor(trimmed.left());
	right = qFloor(trimmed.right()) + 1;
	right = (right > level[i].w_tiles)? level[i].w_tiles: right;
	top = qFloor(trimmed.top());
	bottom = qFloor(trimmed.bottom()) + 1;
	bottom = (bottom > level[i].h_tiles)? level[i].h_tiles: bottom;
	qDebug() << left << top << right << bottom;
	for(i = left; i < right; i++) {
		for(j = top; j < bottom; j++) {
			cur_tile = level[cur_l].tile0 + i +
					(j * level[cur_l].w_tiles);
			if(!tile_loaded[cur_tile]) {
				load_tile(cur_tile);
				qDebug() << " loading tile " << cur_tile;
			}
		}
	}

	qDebug() << zoom << " level: " << cur_l << " rect: " << bounds << trimmed;
	// determine which tiles are being looked at and whether they are
	// currently loaded for current zoom level
}

void MainBox::load_tile(uint64_t id) {
	if(tile_loaded[id]) {
		return;
	}
	int i, j;
	QImage *img;
	QGraphicsPixmapItem *item;
	int ts = LevelData::tile_side;
	int w = ts, h = ts; // used for partial tile sizes
	// determine level
	int cur_l = total_levels - 1;
	for(i = 1; i < total_levels; i++) {
		if(id < level[i].tile0) {
			cur_l = i - 1;
			break;
		}
	}
	qDebug() << "level 1 tile0: " << level[1].tile0 << cur_l << id << i << total_levels;
	LevelData &cur_level = level[cur_l];
	qreal scale = level[0].w / cur_level.w;
	imgbuf = new uint32_t[ts * ts];
	i = ((id - cur_level.tile0) % cur_level.w_tiles);
	j = ((id - cur_level.tile0) / cur_level.w_tiles);
	if(i == cur_level.w_tiles - 1) {
		w = (cur_level.w - i * ts) - 1;
	} else {
		w = ts;
	}
	if(j == cur_level.h_tiles - 1) {
		h = (cur_level.h - j * ts) - 1;
	} else {
		h = ts;
	}
	int x, y;
	x = i * ts * scale;
	y = j * ts * scale;
	qDebug() << "openslide region " << x << y << cur_l << w << h;
	openslide_read_region(osr, imgbuf, x, y, cur_l, w, h);
	img = new QImage((uchar *)imgbuf, w, h, QImage::Format_ARGB32);
	item = scene->addPixmap(QPixmap::fromImage(*img));
	delete img;
	item->setScale(scale);
	item->setPos(i * ts * scale, j * ts * scale);
	tile_loaded[id] = true;
}

