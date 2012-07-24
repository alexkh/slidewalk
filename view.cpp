#include "view.hpp"

#include <QtGui>
#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#endif

#include <qmath.h>
#include <QDebug>

View::View(const QString &name, QWidget *parent) : QFrame(parent) {
	setFrameStyle(Sunken | StyledPanel);
	graphicsView = new ZoomGraphicsView;
	graphicsView->setRenderHint(QPainter::Antialiasing, false);
	graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
	graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
	graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

	int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
	QSize iconSize(size, size);

	QToolButton *zoomInIcon = new QToolButton;
	zoomInIcon->setAutoRepeat(true);
	zoomInIcon->setAutoRepeatInterval(33);
	zoomInIcon->setAutoRepeatDelay(0);
	zoomInIcon->setIcon(QPixmap(":/zoomin.png"));
	zoomInIcon->setIconSize(iconSize);
	QToolButton *zoomOutIcon = new QToolButton;
	zoomOutIcon->setAutoRepeat(true);
	zoomOutIcon->setAutoRepeatInterval(33);
	zoomOutIcon->setAutoRepeatDelay(0);
	zoomOutIcon->setIcon(QPixmap(":/zoomout.png"));
	zoomOutIcon->setIconSize(iconSize);
	zoomSlider = new QSlider;
	zoomSlider->setMinimum(0);
	zoomSlider->setMaximum(499);
	zoomSlider->setValue(499);
	zoomSlider->setTickPosition(QSlider::TicksRight);

	// Zoom slider layout
	QVBoxLayout *zoomSliderLayout = new QVBoxLayout;
	zoomSliderLayout->addWidget(zoomInIcon);
	zoomSliderLayout->addWidget(zoomSlider);
	zoomSliderLayout->addWidget(zoomOutIcon);

	QToolButton *rotateLeftIcon = new QToolButton;
	rotateLeftIcon->setIcon(QPixmap(":/rotateleft.png"));
	rotateLeftIcon->setIconSize(iconSize);
	QToolButton *rotateRightIcon = new QToolButton;
	rotateRightIcon->setIcon(QPixmap(":/rotateright.png"));
	rotateRightIcon->setIconSize(iconSize);
	rotateSlider = new QSlider;
	rotateSlider->setOrientation(Qt::Horizontal);
	rotateSlider->setMinimum(-180);
	rotateSlider->setMaximum(180);
	rotateSlider->setValue(0);
	rotateSlider->setTickPosition(QSlider::TicksBelow);

	// Rotate slider layout
	QHBoxLayout *rotateSliderLayout = new QHBoxLayout;
	rotateSliderLayout->addWidget(rotateLeftIcon);
	rotateSliderLayout->addWidget(rotateSlider);
	rotateSliderLayout->addWidget(rotateRightIcon);

	resetButton = new QToolButton;
	resetButton->setText(tr("0"));
	resetButton->setEnabled(false);

	// Label layout
	QHBoxLayout *labelLayout = new QHBoxLayout;
	label = new QLabel(name);
	antialiasButton = new QToolButton;
	antialiasButton->setText(tr("Antialiasing"));
	antialiasButton->setCheckable(true);
	antialiasButton->setChecked(false);
	openGlButton = new QToolButton;
	openGlButton->setText(tr("OpenGL"));
	openGlButton->setCheckable(true);
#ifndef QT_NO_OPENGL
	openGlButton->setEnabled(QGLFormat::hasOpenGL());
#else
	openGlButton->setEnabled(false);
#endif
	printButton = new QToolButton;
	printButton->setIcon(QIcon(QPixmap(":/fileprint.png")));

	labelLayout->addWidget(label);
	labelLayout->addStretch();
	labelLayout->addWidget(antialiasButton);
	labelLayout->addWidget(openGlButton);
	labelLayout->addWidget(printButton);

	QGridLayout *topLayout = new QGridLayout;
	topLayout->addLayout(labelLayout, 0, 0);
	topLayout->addWidget(graphicsView, 1, 0);
	topLayout->addLayout(zoomSliderLayout, 1, 1);
	topLayout->addLayout(rotateSliderLayout, 2, 0);
	topLayout->addWidget(resetButton, 2, 1);
	setLayout(topLayout);

	connect(resetButton, SIGNAL(clicked()), this, SLOT(resetView()));
	connect(zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setupMatrix()));
	connect(rotateSlider, SIGNAL(valueChanged(int)), this, SLOT(setupMatrix()));
	connect(graphicsView->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(setResetButtonEnabled()));
	connect(graphicsView->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(setResetButtonEnabled()));
	connect(antialiasButton, SIGNAL(toggled(bool)), this, SLOT(toggleAntialiasing()));
	connect(openGlButton, SIGNAL(toggled(bool)), this, SLOT(toggleOpenGL()));
	connect(rotateLeftIcon, SIGNAL(clicked()), this, SLOT(rotateLeft()));
	connect(rotateRightIcon, SIGNAL(clicked()), this, SLOT(rotateRight()));
	connect(zoomInIcon, SIGNAL(clicked()), this, SLOT(zoomIn()));
	connect(zoomOutIcon, SIGNAL(clicked()), this, SLOT(zoomOut()));
	connect(printButton, SIGNAL(clicked()), this, SLOT(print()));

	connect(graphicsView, SIGNAL(zoomChanged(qreal)), this,
						SLOT(zoomChanged(qreal)));

	setupMatrix();
}

ZoomGraphicsView *View::view() const {
	return graphicsView;
}

void View::resetView() {
	zoomSlider->setValue(499);
	rotateSlider->setValue(0);
	setupMatrix();
	graphicsView->ensureVisible(QRectF(0, 0, 0, 0));

	resetButton->setEnabled(false);
}

void View::setResetButtonEnabled() {
	resetButton->setEnabled(true);
}

void View::setupMatrix() {
	qreal scale = qPow(qreal(2), qreal(zoomSlider->value() - 500)
								/ qreal(50));

	QMatrix matrix;
	matrix.scale(scale, scale);
	matrix.rotate(rotateSlider->value());

	graphicsView->setMatrix(matrix);
	setResetButtonEnabled();
}

void View::toggleOpenGL() {
#ifndef QT_NO_OPENGL
	graphicsView->setViewport(openGlButton->isChecked() ? new
		QGLWidget(QGLFormat(QGL::SampleBuffers)) : new QWidget);
#endif
}

void View::toggleAntialiasing() {
	graphicsView->setRenderHint(QPainter::Antialiasing, antialiasButton->isChecked());
}

void View::print() {
#ifndef QT_NO_PRINTER
	QPrinter printer;
	QPrintDialog dialog(&printer, this);
	if(dialog.exec() == QDialog::Accepted) {
		QPainter painter(&printer);
		graphicsView->render(&painter);
	}
#endif
}

void View::zoomIn() {
	zoomSlider->setValue(zoomSlider->value() + 1);
}

void View::zoomOut() {
	zoomSlider->setValue(zoomSlider->value() - 1);
}

void View::rotateLeft() {
	rotateSlider->setValue(rotateSlider->value() - 10);
}

void View::rotateRight() {
	rotateSlider->setValue(rotateSlider->value() + 10);
}

void View::zoomChanged(qreal delta) {
	if(delta > 0) {
		zoomSlider->setValue(zoomSlider->value()
			+ ((delta > 120)? delta / 120: 1));
	} else {
		zoomSlider->setValue(zoomSlider->value()
			+ ((delta < -120)? delta / 120: -1));
	}
//	qDebug() << "hello? " << zoomSlider->value();
}
