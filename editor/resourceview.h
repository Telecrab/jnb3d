#ifndef RESOURCEVIEW_H
#define RESOURCEVIEW_H

#include <QGraphicsView>

class ResourceView : public QGraphicsView
{
    Q_OBJECT

public:
    ResourceView(QWidget *parent = 0);
    void setZoom(qreal value);
    qreal zoom() {return m_zoom;}

protected:
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private:

    qreal m_zoom;
};

#endif // RESOURCEVIEW_H
