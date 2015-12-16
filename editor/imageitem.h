#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include <QGraphicsPixmapItem>

class ImageItem : public QGraphicsPixmapItem
{
public:
    ImageItem(QImage image, QGraphicsItem *parent = 0);

private:
    QImage m_originalImage;

protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
};

#endif // IMAGEITEM_H
