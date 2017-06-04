#include "graph_widget.h"

#include <cassert>
#include <iostream>

#include <QHBoxLayout>
#include <QResizeEvent>

#include "connected_edge.h"

namespace node_editor {

GraphWidget::GraphWidget(QWidget* parent) : QGraphicsView(parent), m_scene(this), m_mouseX(0), m_mouseY(0) {
	setDragMode(QGraphicsView::NoDrag);
	setSizeAdjustPolicy(AdjustIgnored);

	m_scene.setSceneRect(-10000, -10000, 20000, 20000);

	setScene(&m_scene);
	setDragMode(QGraphicsView::RubberBandDrag);
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	setBackgroundBrush(Qt::black);

	setTransformationAnchor(QGraphicsView::NoAnchor);

	setInteractive(true);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

GraphScene& GraphWidget::scene() {
	return m_scene;
}

void GraphWidget::mouseMoveEvent(QMouseEvent* event) {
	// "eat" the left mouse moves while edge editing is in progress
	if(m_scene.isEdgeEditInProgress()) {
		QMouseEvent tmp(event->type(), event->pos(), Qt::NoButton,
		                event->buttons() & ~Qt::LeftButton, event->modifiers());
		QGraphicsView::mouseMoveEvent(&tmp);

		event->accept();
	}
	// translation when mid button is pressed
	else if(event->buttons() & Qt::MiddleButton) {
		const QPointF tr = QPointF(event->x(), event->y()) - QPointF(m_mouseX, m_mouseY);

		translate(tr.x(), tr.y());

		event->accept();
	}
	// default handling
	else
		QGraphicsView::mouseMoveEvent(event);

	// store current mouse position
	m_mouseX = event->x();
	m_mouseY = event->y();
}

}
