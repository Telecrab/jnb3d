#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>
#include <QGraphicsTextItem>
#include <QWheelEvent>
#include "imageitem.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_colorTable(256)
{
    ui->setupUi(this);
    QFile level("jumpbump.dat");
    level.open(QIODevice::ReadOnly);
    if(!level.isOpen()) {
        qFatal("Cannot open 'jumpbump.dat'.");
        return;
    }
    m_datContents = level.readAll(); // Cashing the file in memory for later use
    level.reset();

    // Reading .dat directory
    int numEntries = 0;
    level.read(reinterpret_cast<char*>(&numEntries), 4);
    DatEntry entry;
    QString entryName;
    for (int entryNumber = 0; entryNumber < numEntries; entryNumber++)
    {
        entryName = level.read(12);
        level.read(reinterpret_cast<char*>(&entry), sizeof(DatEntry));
        m_datHeader.insert(entryName, entry);
    }
    QHashIterator<QString, DatEntry> i(m_datHeader);
    int row = 0;
    ui->filesTable->setRowCount(m_datHeader.size());
    while (i.hasNext()) {
        i.next();
        ui->filesTable->setItem(row, 0, new QTableWidgetItem(i.key()));
        ui->filesTable->setItem(row, 1, new QTableWidgetItem(QString("%1").arg(i.value().offset)));
        ui->filesTable->setItem(row, 2, new QTableWidgetItem(QString("%1").arg(i.value().size)));
        row++;
    }
    ui->filesTable->sortByColumn(0, Qt::AscendingOrder);
    ui->filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->graphicsView->setScene(&m_scene);

    //Reading the palette which according to docs should be the same for everything.
    readPCXpalette("level.pcx");
//    m_scene.addPixmap(QPixmap::fromImage(readPCXimage("level.pcx")));

    m_scene.setBackgroundBrush(QBrush(Qt::magenta));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readPCXpalette(const QString &name)
{
    int pcxOffset = m_datHeader.value(name).offset + m_datHeader.value(name).size - 768;
    int paletteIndex = 0;
    QRgb paletteItem;
    while (paletteIndex < 256)
    {
        paletteItem  =                                   (0xFF << 24);
        paletteItem += (uint8_t(m_datContents.at(pcxOffset++)) << 16);
        paletteItem += (uint8_t(m_datContents.at(pcxOffset++)) << 8);
        paletteItem += (uint8_t(m_datContents.at(pcxOffset++)) << 0);
        m_colorTable[paletteIndex++] = paletteItem;
    }
    qDebug() << paletteIndex;
}

QImage MainWindow::readPCXimage(const QString &name)
{
    QImage image(400, 256, QImage::Format_Indexed8); // As described in modify.txt.

    uint8_t *levelImage = image.bits();
    uint8_t currentByte;
    uint8_t fillerByte;
    int pcxOffset = m_datHeader.value(name).offset + 128;
    int levelImageOffset = 0;

    while (levelImageOffset < 400*256) {
        currentByte = m_datContents.at(pcxOffset++);

        if ((currentByte & 0xc0) == 0xc0) {
            fillerByte = m_datContents.at(pcxOffset++);
            currentByte &= 0x3f;

            for (int fillerCount = 0; fillerCount < currentByte; fillerCount++) {
                levelImage[levelImageOffset++] = fillerByte;
            }

        } else {
            levelImage[levelImageOffset++] = currentByte;
        }
    }

    image.setColorTable(m_colorTable);

    return image;
}

std::vector<GobImage> MainWindow::readGOB(const QString &name)
{
    uint32_t offset = m_datHeader.value(name).offset;
    GobHeader header;

    header.numberOfImages = readUint16(offset);

    for(int i = 0; i < header.numberOfImages; ++i) {
        uint32_t value = readUint32(offset);
        header.imageOffsets.push_back(value);
    }
    header.imageOffsets.push_back(m_datHeader.value(name).size); // The GOB end offset

    std::vector<GobImage> gobs;
    for(int i = 0; i < header.numberOfImages; ++i) {
        GobImage gobImage;
        std::vector<uint8_t>::size_type entrySize = header.imageOffsets[i+1] - header.imageOffsets[i];
        gobImage.buf_ = reinterpret_cast<const uint8_t*>( &(m_datContents.constData()[ m_datHeader.value(name).offset + header.imageOffsets[i] ]) );
        gobs.push_back(gobImage);
    }

    return gobs;
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

void MainWindow::on_filesTable_cellClicked(int row, int column)
{
    QString resourceName = ui->filesTable->item(row, 0)->text();
    QRectF boundingBox;
    m_scene.clear();

    if(resourceName.endsWith(".pcx")) {
        ImageItem *item = new ImageItem(readPCXimage(resourceName));
        m_scene.addItem(item);
        boundingBox = item->boundingRect();
    }

    if(resourceName.endsWith(".gob")) {
        std::vector<GobImage> gobImages = readGOB(resourceName);

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
    }

    m_scene.setSceneRect(boundingBox);
    ui->graphicsView->setZoom(1.0);
    ui->graphicsView->centerOn(boundingBox.center());
}

uint16_t MainWindow::readUint16(uint32_t &offset)
{
    uint16_t result;
    result  = (uint8_t(m_datContents.at(offset++)) << 0);
    result += (uint8_t(m_datContents.at(offset++)) << 8);
    return result;
}

uint32_t MainWindow::readUint32(uint32_t &offset)
{
    uint32_t result;
    result  = (uint8_t(m_datContents.at(offset++)) << 0);
    result += (uint8_t(m_datContents.at(offset++)) << 8);
    result += (uint8_t(m_datContents.at(offset++)) << 16);
    result += (uint8_t(m_datContents.at(offset++)) << 24);
    return result;
}
