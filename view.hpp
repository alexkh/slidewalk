#ifndef VIEW_HPP
#define VIEW_HPP

#include <QGraphicsView>
#include <QFrame>
#include "zoomgraphicsview.hpp"

QT_FORWARD_DECLARE_CLASS(QLabel)
QT_FORWARD_DECLARE_CLASS(QSlider)
QT_FORWARD_DECLARE_CLASS(QToolButton)

class View : public QFrame {
	Q_OBJECT
public:
	View(const QString &name, QWidget *parent = 0);

	ZoomGraphicsView *view() const;
	QSlider *zoomSlider;
	QSlider *rotateSlider;

private slots:
	void resetView();
	void setResetButtonEnabled();
	void setupMatrix();
	void toggleOpenGL();
	void toggleAntialiasing();
	void print();

	void zoomIn();
	void zoomOut();
	void rotateLeft();
	void rotateRight();
	void zoomChanged(qreal);

private:
	ZoomGraphicsView *graphicsView;
	QLabel *label;
	QToolButton *openGlButton;
	QToolButton *antialiasButton;
	QToolButton *printButton;
	QToolButton *resetButton;

};

#endif // VIEW_HPP
