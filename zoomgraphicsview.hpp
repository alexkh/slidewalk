#ifndef ZOOMGRAPHICSVIEW_HPP
#define ZOOMGRAPHICSVIEW_HPP

#include <QGraphicsView>
#include <QWheelEvent>

class ZoomGraphicsView : public QGraphicsView {
Q_OBJECT
public:
	explicit ZoomGraphicsView(QWidget *parent = 0);
	//Set the current centerpoint in the
	void SetCenter(const QPointF& centerPoint);
	QPointF GetCenter() { return CurrentCenterPoint; }

signals:
	void zoomChanged(qreal);

public slots:

protected:
	// Holds the current centerpoint for the view,
	// used for panning and zooming
	QPointF CurrentCenterPoint;

	// From panning the view
	QPoint LastPanPoint;

	//Take over the interaction
	virtual void mouseDoubleClickEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void wheelEvent(QWheelEvent* event);
};

#endif // ZOOMGRAPHICSVIEW_HPP
