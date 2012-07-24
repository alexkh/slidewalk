#ifndef MAINBOX_HPP
#define MAINBOX_HPP

#include <stdint.h>
#include <QMainWindow>
#include <QFileDialog>
#include <QStatusBar>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <qmath.h>
#include <QSettings>
#include <QFileInfo>

#include "view.hpp"

extern "C" {
	#include "openslide.h"
}

namespace Ui {
	class MainBox;
}

// this is a quick access metric data structure, so it has some redundant data
// each magnification level
// is split into
// imaginary tiles, tile_side x tile_side pixels big,
// each tile has an id counting from tile 0 of level 0
class LevelData {
public:
	static const int tile_side = 2048; // each tile is side x side pixels
	int64_t w, h; // width and height
	qreal scale; // magnification relative to level0
	int max_zoom; // maximum zoom at which there is no pixelation effect
	int64_t w_tiles, h_tiles; // tiles in both direction
	uint64_t tile0; // id of first tile: corresponds to its position in an
			// 1-dimentional array of tile states which we'll use to
			// know which tiles have been loaded/unloaded
	uint64_t recalc(uint64_t tile_id) {
		// this function assumes that w, h, scale and max_zoom are set
		// returns next available tile id
		w_tiles = qCeil(qreal(w) / qreal(tile_side));
		h_tiles = qCeil(qreal(h) / qreal(tile_side));
		tile0 = tile_id;
		return tile0 + (w_tiles * h_tiles);
	}
};

class MainBox : public QMainWindow {
Q_OBJECT

public:
	explicit MainBox(QWidget *parent = 0);
	~MainBox();

private slots:
	void on_actionFileOpen_triggered();
	void on_zoom_changed(int);

protected:
	void closeEvent(QCloseEvent *event);

private:
	void writeSettings();
	void writeSetting(const QString &key,
				const QVariant &value, bool sync = false);
	void readSettings();
	void load_tile(uint64_t id);

	Ui::MainBox *ui;
	QString slideFileName;
	openslide_t *osr;
	uint32_t *imgbuf;
	QGraphicsScene *scene;
	View *view;
	int total_levels; // how many levels of magnification in the file
	LevelData *level; // starting tile for each level
	bool *tile_loaded; // an array of all tiles holding status of each
	uint64_t total_tiles; // how many tiles are in all levels together
	QString curDir; // holds the name of the last used dir for easy access
};

#endif // MAINBOX_HPP
