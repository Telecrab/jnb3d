#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QMainWindow>
#include <QImage>
#include <QHash>
#include <QGraphicsScene>
#include <vector>

struct DatEntry
{
    uint32_t offset;
    uint32_t size;
};
Q_DECLARE_TYPEINFO(DatEntry, Q_PRIMITIVE_TYPE);

struct DatHeader
{
    uint32_t nummerOfEntries;
    DatEntry *entries;
};
Q_DECLARE_TYPEINFO(DatHeader, Q_PRIMITIVE_TYPE);

// Also see http://stackoverflow.com/a/25938871.
struct GobImage {
    const uint8_t *buf_;

    const uint16_t& width() const { return reinterpret_cast<const uint16_t*>(buf_)[0]; }
    const uint16_t& height() const { return reinterpret_cast<const uint16_t*>(buf_)[1]; }
    const uint16_t& hotSpotX() const { return reinterpret_cast<const uint16_t*>(buf_)[2]; }
    const uint16_t& hotSpotY() const { return reinterpret_cast<const uint16_t*>(buf_)[3]; }

    const uint8_t *bitmap() const { return &buf_[8]; }
};

struct GobHeader
{
    uint16_t numberOfImages;
    std::vector<uint32_t> imageOffsets;
};

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
    void on_filesTable_cellClicked(int row, int column);

private:
    Ui::MainWindow *ui;
    QHash<QString, DatEntry> m_datHeader;
    QByteArray m_datContents;
    QGraphicsScene m_scene;
    QVector<QRgb> m_colorTable;

    uint16_t readUint16(uint32_t &offset);
    uint32_t readUint32(uint32_t &offset);

    QImage readPCX(const QString &name);
    std::vector<GobImage> readGOB(const QString &name);
    QImage readGobImage(const GobImage &gobImage);

};

#endif // LAUNCHER_H
