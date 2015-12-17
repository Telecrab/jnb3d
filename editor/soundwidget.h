#ifndef SOUNDWIDGET_H
#define SOUNDWIDGET_H

#include <QWidget>
#include <QBuffer>
#include <QtMultimedia/QAudio>

QT_BEGIN_NAMESPACE
class QAudioOutput;
QT_END_NAMESPACE

namespace Ui {
class SoundWidget;
}

class SoundWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SoundWidget(QWidget *parent = 0);
    ~SoundWidget();

    void setSoundData(QByteArray data);
    void setSoundDescription(const QString &description);

private slots:
    void on_playPushButton_clicked();
    void handleStateChanged(QAudio::State newState);

private:
    Ui::SoundWidget *ui;

    QBuffer m_soundBuffer;
    QAudioOutput *m_audioOutput;
};

#endif // SOUNDWIDGET_H
