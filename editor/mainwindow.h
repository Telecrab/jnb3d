#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QMainWindow>
#include <QImage>
#include <QHash>
#include <QGraphicsScene>
#include <vector>

#include "resourcecontainer.h"

struct DatEntry
{
    uint32_t offset;
    uint32_t size;
};
Q_DECLARE_TYPEINFO(DatEntry, Q_PRIMITIVE_TYPE);

struct DatHeader
{
    uint32_t numberOfEntries;
    DatEntry *entries;
};
Q_DECLARE_TYPEINFO(DatHeader, Q_PRIMITIVE_TYPE);

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_filesTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

private:
    ResourceContainer m_resourceContainer;
    Ui::MainWindow *ui;
    QHash<QString, DatEntry> m_datHeader;
    QGraphicsScene m_scene;
    QVector<QRgb> m_colorTable;

    QImage readPCXimage(const QString &name);
    QVector<QRgb> readPCXpalette(const QString &name);
    QImage readGobImage(const GobImage &gobImage);
    QByteArray readSMP(const QString &name);
    QByteArray readMOD(const QString &name);

};

#endif // LAUNCHER_H
