#include "zoomgraphicsview.hpp"

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTextStream>
#include <QScrollBar>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

ZoomGraphicsView::ZoomGraphicsView(QWidget *parent) :
QGraphicsView(parent) {
	setRenderHints(QPainter::Antialiasing |
					QPainter::SmoothPixmapTransform);

	//Set-up the scene
	QGraphicsScene* Scene = new QGraphicsScene(this);
	setScene(Scene);

	//Populate the scene
	for(int x = 0; x < 1000; x = x + 25) {
		for(int y = 0; y < 1000; y = y + 25) {
			if(x % 100 == 0 && y % 100 == 0) {
				Scene->addRect(x, y, 2, 2);
				QString pointString;
				QTextStream stream(&pointString);
				stream << "(" << x << "," << y << ")";
				QGraphicsTextItem* item =
						Scene->addText(pointString);
				item->setPos(x, y);
			} else {
				//Scene->addRect(x, y, 1, 1);
			}
		}
	}

	//Set-up the view
	setSceneRect(0, 0, 1000, 1000);
	SetCenter(QPointF(500.0, 500.0)); //A modified version of centerOn(),
						//handles special cases
	setCursor(Qt::OpenHandCursor);
}

/**
  * Sets the current centerpoint.  Also updates the scene's center point.
  * Unlike centerOn, which has no way of getting the floating point center
  * back, SetCenter() stores the center point.  It also handles the special
  * sidebar case.  This function will claim the centerPoint to sceneRec ie.
  * the centerPoint must be within the sceneRec.
  */
//Set the current centerpoint in the
void ZoomGraphicsView::SetCenter(const QPointF& centerPoint) {
	//Get the rectangle of the visible area in scene coords
	QRectF visibleArea = mapToScene(rect()).boundingRect();

	//Get the scene area
	QRectF sceneBounds = sceneRect();

	double boundX = visibleArea.width() / 2.0;
	double boundY = visibleArea.height() / 2.0;
	double boundWidth = sceneBounds.width() - 2.0 * boundX;
	double boundHeight = sceneBounds.height() - 2.0 * boundY;

	//The max boundary that the centerPoint can be to
	QRectF bounds(boundX, boundY, boundWidth, boundHeight);
	CurrentCenterPoint = centerPoint;
/*
	if(bounds.contains(centerPoint)) {
		//We are within the bounds
		CurrentCenterPoint = centerPoint;
	} else {
	        //We need to clamp or use the center of the screen
		if(visibleArea.contains(sceneBounds)) {
			//Use the center of scene ie. we can see the whole scene
			CurrentCenterPoint = sceneBounds.center();
		} else {
			CurrentCenterPoint = centerPoint;
			//We need to clamp the center.
			//The centerPoint is too large
			if(centerPoint.x() > bounds.x() + bounds.width()) {
				CurrentCenterPoint.setX(bounds.x()
							+ bounds.width());
			} else if(centerPoint.x() < bounds.x()) {
				CurrentCenterPoint.setX(bounds.x());
			}
			if(centerPoint.y() > bounds.y() + bounds.height()) {
				CurrentCenterPoint.setY(bounds.y()
							+ bounds.height());
			} else if(centerPoint.y() < bounds.y()) {
				CurrentCenterPoint.setY(bounds.y());
			}
		}
	}
*/
	//Update the scrollbars
	centerOn(CurrentCenterPoint);
}

void ZoomGraphicsView::mouseDoubleClickEvent(QMouseEvent *event) {
	SetCenter(mapToScene(event->pos()));
	int delta;
	if(event->button() == Qt::LeftButton) {
		delta = 1200;
	} else {
		delta = -1200;
	}
	zoomChanged(delta);
}

/**
  * Handles when the mouse button is pressed
  */
void ZoomGraphicsView::mousePressEvent(QMouseEvent* event) {
	//For panning the view
	LastPanPoint = event->pos();
	setCursor(Qt::ClosedHandCursor);
}

/**
  * Handles when the mouse button is released
  */
void ZoomGraphicsView::mouseReleaseEvent(QMouseEvent* event) {
	setCursor(Qt::OpenHandCursor);
	LastPanPoint = QPoint();
}

/**
*Handles the mouse move event
*/
void ZoomGraphicsView::mouseMoveEvent(QMouseEvent* event) {
	if(!LastPanPoint.isNull()) {
		//Get how much we panned
		QPointF delta = mapToScene(LastPanPoint)
						- mapToScene(event->pos());
		LastPanPoint = event->pos();
		//Update the center ie. do the pan
		SetCenter(GetCenter() + delta);
	}
}

/**
  * Zoom the view in and out.
  */
void ZoomGraphicsView::wheelEvent(QWheelEvent* event) {
/*	//Get the position of the mouse before scaling, in scene coords
	QPointF pointBeforeScale(mapToScene(event->pos()));
	//Get the original screen centerpoint
	QPointF screenCenter = GetCenter();
	//Scale the view ie. do the zoom
	double scaleFactor = 1.2; //How fast we zoom
	if(event->delta() > 0) {
		//Zoom in
	} else {
		//Zooming out
		scaleFactor = 1.0 / scaleFactor;
	}
//	scale(scaleFactor, scaleFactor);

	//Get the position after scaling, in scene coords
	QPointF pointAfterScale(mapToScene(event->pos()));

	//Get the offset of how the screen moved
	QPointF offset = pointBeforeScale - pointAfterScale;

	//Adjust to the new center for correct zooming
//	QPointF newCenter = screenCenter + offset;
//	QPointF newCenter = pointBeforeScale;
//	qDebug() << newCenter;
//	SetCenter(newCenter);
*/
	zoomChanged(event->delta());
}


