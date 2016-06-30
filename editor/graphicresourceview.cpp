#include "graphicresourceview.h"
#include <QWheelEvent>
#include <cmath>

GraphicResourceView::GraphicResourceView(QWidget *parent)
    : QGraphicsView(parent) ,
      m_zoom(1.0)
{
//    setDragMode(QGraphicsView::ScrollHandDrag);

}

void GraphicResourceView::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers() & Qt::ControlModifier) {
        if (event->delta() > 0)
            setZoom(m_zoom + 0.1);
        else
            setZoom(m_zoom - 0.1);
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void GraphicResourceView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Alt) {
        setDragMode(QGraphicsView::ScrollHandDrag);
        event->accept();
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

void GraphicResourceView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Alt) {
        setDragMode(QGraphicsView::NoDrag);
        event->accept();
    } else {
        QGraphicsView::keyReleaseEvent(event);
    }
}

void GraphicResourceView::setZoom(qreal value)
{
    if( (value < 0.01) || ( std::abs(m_zoom - value) < 0.0001 ) )
        return;

    m_zoom = value;
    QMatrix matrix;
    matrix.scale(value, value);

    setMatrix(matrix);
}

