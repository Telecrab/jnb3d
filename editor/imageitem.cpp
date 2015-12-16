#include "imageitem.h"
#include <QToolTip>
#include <QGraphicsSceneMouseEvent>

ImageItem::ImageItem(QImage image, QGraphicsItem *parent)
    : QGraphicsPixmapItem(QPixmap::fromImage(image), parent),
      m_originalImage(image)
{
    setFlags(ItemIsSelectable /*| ItemIsMovable*/);
    setAcceptHoverEvents(true);
    setShapeMode(BoundingRectShape);
}

void ImageItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{

    QPoint screenPosition = event->screenPos();
    QPoint itemPosition = event->pos().toPoint();

    QString toolTip = QString("Pixel color index: %1\nPixel color: 0x%2")
            .arg(m_originalImage.pixelIndex(itemPosition)).arg(m_originalImage.pixel(itemPosition), 8, 16, QChar('0'));
    QToolTip::hideText();
    QToolTip::showText(screenPosition,  toolTip);

    QGraphicsItem::hoverMoveEvent(event);
}
