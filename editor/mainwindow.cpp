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

    m_datLoader.loadArchive("jumpbump.dat");
    m_resourceContainer.loadContainer("jumpbump.dat");

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
//    EntrySize entrySize;
//    char *entryData = m_datLoader.getEntryData( name.toStdString(), entrySize );
//    int pcxOffset = entrySize - 768; // Palette is the last 768 bytes of the image.
//    int paletteIndex = 0;
//    QVector<QRgb> palette(256);
//    QRgb paletteItem;
//    while (paletteIndex < 256)
//    {
//        paletteItem  =                                   (0xFF << 24);
//        paletteItem += ( uint8_t( entryData[pcxOffset++] ) << 16 );
//        paletteItem += ( uint8_t( entryData[pcxOffset++] ) << 8 );
//        paletteItem += ( uint8_t( entryData[pcxOffset++] ) << 0 );
//        palette[paletteIndex++] = paletteItem;
//    }

//    return palette;
    return QVector<QRgb>::fromStdVector( m_resourceContainer.readPCXpalette( name.toStdString() ) );
}

QImage MainWindow::readPCXimage(const QString &name)
{
    QImage image(400, 256, QImage::Format_Indexed8); // As described in modify.txt.

//    EntrySize entrySize;
//    char *entryData = m_datLoader.getEntryData( name.toStdString(), entrySize );
//    uint8_t *levelImage = image.bits();
//    uint8_t currentByte;
//    uint8_t fillerByte;
//    int pcxOffset = 128;
//    int levelImageOffset = 0;

//    while (levelImageOffset < 400 * 256) {
//        currentByte = entryData[pcxOffset++];

//        if ((currentByte & 0xc0) == 0xc0) {
//            fillerByte = entryData[pcxOffset++];
//            currentByte &= 0x3f;

//            for (int fillerCount = 0; fillerCount < currentByte; fillerCount++) {
//                levelImage[levelImageOffset++] = fillerByte;
//            }

//        } else {
//            levelImage[levelImageOffset++] = currentByte;
//        }
//    }
    std::vector<ColorIndex> pcx = m_resourceContainer.readPCXimage( name.toStdString() );
    memcpy( image.bits(), pcx.data(), pcx.size() * sizeof(ColorIndex) );
    image.setColorTable(readPCXpalette(name));

    return image;
}

std::vector<GobImage> MainWindow::readGOB(const QString &name)
{
    EntrySize entrySize;
    char *entryData = m_datLoader.getEntryData( name.toStdString(), entrySize );
    char *entryDataBegin = entryData;
    GobHeader header;

    header.numberOfImages = readUint16(entryData);

    for(int i = 0; i < header.numberOfImages; ++i) {
        uint32_t value = readUint32(entryData);
        header.imageOffsets.push_back(value);
    }
//    header.imageOffsets.push_back(entrySize); // The GOB end offset

    std::vector<GobImage> gobs;
    for(int i = 0; i < header.numberOfImages; ++i) {
        GobImage gobImage;
//        std::vector<uint8_t>::size_type entrySize = header.imageOffsets[i+1] - header.imageOffsets[i];
        gobImage.buf_ = reinterpret_cast<const uint8_t*>( &entryDataBegin[ header.imageOffsets[i] ] );
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

QByteArray MainWindow::readSMP(const QString &name)
{
    EntrySize entrySize;
    char *entryData = m_datLoader.getEntryData( name.toStdString(), entrySize );
    QByteArray data( entryData, entrySize );

    // Converting signed to unsigned format, because signed doesn't play properly.
    for(int i = 0; i < data.size(); ++i) {
        data[i] = data[i] + char(127);
    }

    return data;
}

QByteArray MainWindow::readMOD(const QString &name)
{
    EntrySize entrySize;
    char *entryData = m_datLoader.getEntryData( name.toStdString(), entrySize );
    QByteArray data( entryData, entrySize );
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
        ui->stackedWidget->setCurrentWidget(ui->graphicsPage);
    }

    if(resourceName == "levelmap.txt") {
        ImageItem *item = new ImageItem(readPCXimage("level.pcx"));
        m_scene.addItem(item);
        boundingBox = item->boundingRect();

        EntrySize entrySize;
        char *entryData = m_datLoader.getEntryData( resourceName.toStdString(), entrySize );
        for(int row = 0; row < 16; row++) {
            for(int column = 0; column < 22; column++) {
                QColor blockColor;

                switch(*entryData) {
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
                entryData++;
            }
            entryData += 2;
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

uint16_t MainWindow::readUint16(char* &data)
{
    uint16_t result;
    result  = ( uint8_t(*data++) << 0 );
    result += ( uint8_t(*data++) << 8 );
    return result;
}

uint32_t MainWindow::readUint32(char* &data)
{
    uint32_t result;
    result  = ( uint8_t(*data++) << 0  );
    result += ( uint8_t(*data++) << 8  );
    result += ( uint8_t(*data++) << 16 );
    result += ( uint8_t(*data++) << 24 );
    return result;
}
