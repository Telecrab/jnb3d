#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>
#include <QGraphicsTextItem>
#include <QWheelEvent>

#include "imageitem.h"
#include "qtfileio.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_colorTable(256)
{
    ui->setupUi(this);

    m_resourceContainer.loadContainer( std::make_shared<QtFileIO>("jumpbump.dat") );

    // Reading .dat directory
    std::vector<ArchiveEntry> archiveContents = m_resourceContainer.containerContents();
    for (ArchiveEntry archiveEntry : archiveContents)
    {
        DatEntry entry;
        QString entryName = QString::fromStdString( archiveEntry.name );
        entry.offset = archiveEntry.offset;
        entry.size   = archiveEntry.size;
        m_datHeader.insert(entryName, entry);
    }

    QHashIterator<QString, DatEntry> i(m_datHeader);
    int row = 0;
    ui->filesTable->setRowCount(m_datHeader.size());
    while (i.hasNext()) {
        i.next();

        Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;

        QTableWidgetItem * item = new QTableWidgetItem(i.key());
        item->setFlags(flags | Qt::ItemIsEditable);
        ui->filesTable->setItem(row, 0, item);

        item = new QTableWidgetItem(QString("%1").arg(i.value().offset));
        item->setFlags(flags);
        ui->filesTable->setItem(row, 1, item);

        item =  new QTableWidgetItem(QString("%1").arg(i.value().size));
        item->setFlags(flags);
        ui->filesTable->setItem(row, 2, item);

        row++;
    }
    ui->filesTable->sortByColumn(0, Qt::AscendingOrder);
    ui->filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->graphicsView->setScene(&m_scene);

    //Reading the palette which will be used for GOBs.
    m_colorTable = readPCXpalette("level.pcx");

    m_scene.setBackgroundBrush(QBrush(Qt::magenta));

    ui->stackedWidget->setCurrentWidget(ui->emptyPage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QVector<QRgb> MainWindow::readPCXpalette(const QString &name)
{
    return QVector<QRgb>::fromStdVector( m_resourceContainer.readPCXpalette( name.toStdString() ) );
}

QImage MainWindow::readPCXimage(const QString &name)
{
    QImage image(400, 256, QImage::Format_Indexed8); // As described in modify.txt.
    std::vector<ColorIndex> pcx = m_resourceContainer.readPCXimage( name.toStdString() );

    memcpy( image.bits(), pcx.data(), pcx.size() * sizeof(ColorIndex) );
    image.setColorTable(readPCXpalette(name));

    return image;
}

QImage MainWindow::readGobImage(const GobImage &gobImage)
{
    QImage result(gobImage.width(), gobImage.height(), QImage::Format_Indexed8);
    result.setColorTable(m_colorTable);
    result.setColor(0, Qt::transparent);

    //QImage aligns scanLines to a 32-bit boundary, so we can't just copy everything to result.bits().
    for(int line = 0; line < result.height(); ++line) {
            memcpy( result.scanLine(line), &gobImage.bitmap()[line * result.width()], size_t(result.width()) );
    }

    return result;
}

QByteArray MainWindow::readSMP(const QString &name)
{
    std::vector<char> tmp = m_resourceContainer.readSMP( name.toStdString() );
    QByteArray data( tmp.data(), tmp.size() );

    // Converting signed to unsigned format, because signed doesn't play properly.
    for(int i = 0; i < data.size(); ++i) {
        data[i] = data[i] + char(127);
    }

    return data;
}

QByteArray MainWindow::readMOD(const QString &name)
{
    std::vector<char> tmp = m_resourceContainer.readMOD( name.toStdString() );
    QByteArray data( tmp.data(), tmp.size() );
    return data;
}

void MainWindow::on_filesTable_currentCellChanged(int currentRow, int /*currentColumn*/, int /*previousRow*/, int /*previousColumn*/)
{
    ui->stackedWidget->setCurrentWidget(ui->emptyPage);

    QString resourceName = ui->filesTable->item(currentRow, 0)->text();
    QRectF boundingBox;
    m_scene.clear();

    if(resourceName.endsWith(".pcx")) {
        ImageItem *item = new ImageItem(readPCXimage(resourceName));
        m_scene.addItem(item);
        boundingBox = item->boundingRect();
        ui->stackedWidget->setCurrentWidget(ui->graphicsPage);
    }

    if(resourceName.endsWith(".gob")) {
        std::vector<GobImage> gobImages = m_resourceContainer.readGOB( resourceName.toStdString() );

        QPointF coordinates;
        for(const GobImage &gobImage : gobImages) {
            ImageItem *item = new ImageItem(readGobImage(gobImage));
            m_scene.addItem(item);
            item->setPos(coordinates);
            coordinates.rx() += gobImage.width() + 2;

            QRectF itemBB = item->mapToScene(item->boundingRect()).boundingRect();
            boundingBox.setRight( qMax( boundingBox.right(), itemBB.right() ) );
            boundingBox.setBottom( qMax( boundingBox.bottom(), item->boundingRect().bottom() ) );
            m_scene.addRect(itemBB, QPen(QColor(127, 127, 127, 200)));
        }
        ui->stackedWidget->setCurrentWidget(ui->graphicsPage);
    }

    if(resourceName == "levelmap.txt") {
        ImageItem *item = new ImageItem(readPCXimage("level.pcx"));
        m_scene.addItem(item);
        boundingBox = item->boundingRect();

        std::vector<char> entryData = m_resourceContainer.readLevel( resourceName.toStdString() );
        std::vector<char>::size_type index = 0;
        for(int row = 0; row < 16; row++) {
            for(int column = 0; column < 22; column++) {
                QColor blockColor;

                switch( entryData[index] ) {
                case '0':
                    blockColor.setRgb(0, 0, 0, 0);
                    break;

                case '1':
                    blockColor.setRgb(200, 100, 0, 200);
                    break;

                case '2':
                    blockColor.setRgb(0, 60, 85, 200);
                    break;

                case '3':
                    blockColor.setRgb(140, 220, 255, 200);
                    break;

                case '4':
                    blockColor.setRgb(200, 200, 0, 200);
                    break;

                default:
                    blockColor.setRgb(200, 0, 0, 200);
                }

                QPen pen;
                pen.setWidth(0);
                m_scene.addRect(QRect(column * 16, row * 16, 16, 16), QPen(pen), QBrush(blockColor));
                index++;
            }
            index += 2; // Skip "\r\n" at the end of line;
        }
        ui->stackedWidget->setCurrentWidget(ui->graphicsPage);
    }

    m_scene.setSceneRect(boundingBox);
    ui->graphicsView->setZoom(1.0);
    ui->graphicsView->centerOn(boundingBox.center());

    if(resourceName.endsWith(".smp")) {
        ui->stackedWidget->setCurrentWidget(ui->soundPage);
        ui->soundPage->setSoundData(readSMP(resourceName));
        ui->soundPage->setSoundDescription("Raw 8 bit signed 22050Hz");
    }

    if(resourceName.endsWith(".mod")) {
        ui->stackedWidget->setCurrentWidget(ui->musicPage);
        ui->musicPage->setMusicData(readMOD(resourceName));
    }
}
