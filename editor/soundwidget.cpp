#include "soundwidget.h"
#include "ui_soundwidget.h"

#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioFormat>
#include <QDebug>

SoundWidget::SoundWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundWidget)
{
    ui->setupUi(this);

    QAudioFormat format;
    format.setSampleRate(22050);
    format.setChannelCount(1);
    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }

    m_audioOutput = new QAudioOutput(format, this);
    connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
}

SoundWidget::~SoundWidget()
{
    delete ui;
    delete m_audioOutput;
}

void SoundWidget::setSoundData(QByteArray data)
{
    int period = m_audioOutput->periodSize(); qDebug() <<period;

    if(period) {
        int padding = period - ( data.size() % period );
        data.resize(data.size() + padding + 1);
    }

    m_audioOutput->stop();
    m_soundBuffer.close();
    m_soundBuffer.setData(data);
}

void SoundWidget::setSoundDescription(const QString & description)
{
    ui->soundInfoLabel->setText(description);
}

void SoundWidget::on_playPushButton_clicked()
{
    m_audioOutput->stop();
    m_soundBuffer.close();

    // TODO: limit maximum size of the audio buffer.
    qDebug() << m_audioOutput->bufferSize();
    m_audioOutput->setBufferSize(m_soundBuffer.buffer().size());

    m_soundBuffer.open(QIODevice::ReadOnly);
    m_audioOutput->start(&m_soundBuffer);
    qDebug() << m_audioOutput->bufferSize();
}

void SoundWidget::handleStateChanged(QAudio::State newState)
{
    switch (newState) {
        case QAudio::IdleState:
            // Finished playing (no more data)
            m_audioOutput->stop();
            break;

        case QAudio::StoppedState:
            // Stopped for other reasons
            if (m_audioOutput->error() != QAudio::NoError) {
                // Error handling
                qDebug() << m_audioOutput->error();
            }
            break;

        default:
            break;
    }
}
