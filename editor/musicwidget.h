#ifndef MUSICWIDGET_H
#define MUSICWIDGET_H

#include <QWidget>
#include <QBuffer>
#include <QtMultimedia/QAudio>

QT_BEGIN_NAMESPACE
class QAudioOutput;
QT_END_NAMESPACE

namespace Ui {
class MusicWidget;
}

namespace ItsASecretToEverybody {
class MicromodDevice;
}

class MusicWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MusicWidget(QWidget *parent = 0);
    ~MusicWidget();

    void setMusicData(const QByteArray &data);

public slots:
    void handleStateChanged(QAudio::State newState);

private slots:
    void on_pushButton_toggled(bool checked);

    void on_sliderSongPosition_valueChanged(int value);

private:
    Ui::MusicWidget *ui;

    QAudioOutput *m_audioOutput;
    ItsASecretToEverybody::MicromodDevice *m_micromod;
};

#endif // MUSICWIDGET_H
