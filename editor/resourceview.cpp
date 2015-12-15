#include "resourceview.h"
#include <QWheelEvent>

ResourceView::ResourceView(QWidget *parent)
    : QGraphicsView(parent) ,
      m_zoom(1.0)
{
//    setDragMode(QGraphicsView::ScrollHandDrag);

}

void ResourceView::wheelEvent(QWheelEvent *event)
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

void ResourceView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Alt) {
        setDragMode(QGraphicsView::ScrollHandDrag);
        event->accept();
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

void ResourceView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Alt) {
        setDragMode(QGraphicsView::NoDrag);
        event->accept();
    } else {
        QGraphicsView::keyReleaseEvent(event);
    }
}

void ResourceView::setZoom(qreal value)
{
    if( (value < 0.01) || (std::abs(m_zoom - value) < 0.0001) )
        return;

    m_zoom = value;
    QMatrix matrix;
    matrix.scale(value, value);

    setMatrix(matrix);
}

