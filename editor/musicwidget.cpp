#include "musicwidget.h"
#include "ui_musicwidget.h"

#include <QIODevice>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioFormat>
#include <QSysInfo>
#include <QDebug>

extern "C" {
#include "micromod/micromod.h"
}

typedef qint64 milliseconds;

namespace ItsASecretToEverybody {
class MicromodDevice : public QIODevice
{
    Q_OBJECT

public:
    MicromodDevice(const QAudioFormat &format);

    bool open(const QByteArray &musicData);
    void close() Q_DECL_OVERRIDE;
    bool rewind();
    bool setSongPosition(milliseconds position);

    qint64 readData(char *data, qint64 maxlen) Q_DECL_OVERRIDE;
    qint64 writeData(const char *data, qint64 len) Q_DECL_OVERRIDE;
    qint64 bytesAvailable() const Q_DECL_OVERRIDE;

    QString songInfo();
    int songDurationMilliseconds() { return double(m_samplesRemaining) * 1000 / m_format.sampleRate();}

private:
    QAudioFormat m_format;
    uint m_sampleSize;
    qint64 m_samplesRemaining;
    QByteArray m_musicData;
};

#include "musicwidget.moc" // Because you can't just have Q_OBJECT declared in the *.cpp.

MicromodDevice::MicromodDevice(const QAudioFormat &format)
    : m_format(format),
      m_sampleSize( format.channelCount() * (format.sampleSize() / 8) ),
      m_samplesRemaining(0)
{

}

bool MicromodDevice::open(const QByteArray &musicData)
{
    m_musicData = musicData;
    if( micromod_initialise( reinterpret_cast<signed char*>(m_musicData.data()), m_format.sampleRate() ) ) {
        qDebug("Micromod has set up us the bomb!");
        return false;
    }

    m_samplesRemaining = micromod_calculate_song_duration();

    return QIODevice::open(QIODevice::ReadOnly);
}

void MicromodDevice::close()
{
    m_samplesRemaining = 0;
    QIODevice::close();
}

bool MicromodDevice::rewind()
{
    if( !isOpen() ) return false;

    m_samplesRemaining = micromod_calculate_song_duration();
    micromod_set_position(0);
    return true;
}

bool MicromodDevice::setSongPosition(milliseconds position)
{
    if(position < 0) return false;

    rewind();

    long samplesToSkip = m_format.sampleRate() / 1000 * position;
    micromod_get_audio(NULL, samplesToSkip);
    m_samplesRemaining -= samplesToSkip;

    return true;
}

qint64 MicromodDevice::readData(char *data, qint64 maxlen)
{
    long samplesToProcess;
    samplesToProcess = maxlen / m_sampleSize;

    memset(data, 0, maxlen);

    if(m_samplesRemaining < samplesToProcess) {
        samplesToProcess = m_samplesRemaining;
    }

    if( samplesToProcess <= 0 ) {

        return -1;

    } else {

        short *mixBuffer = reinterpret_cast<short*>( data );
        micromod_get_audio( mixBuffer, samplesToProcess );
        m_samplesRemaining -= samplesToProcess;
        return maxlen; //TODO: For some reason some milliseconds of song are truncated at the end.

    }

}

qint64 MicromodDevice::writeData(const char *data, qint64 len)
{    
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 MicromodDevice::bytesAvailable() const
{
    return m_samplesRemaining * m_sampleSize;
}

QString MicromodDevice::songInfo()
{
    QString songInfo;
    int inst;
    char string[ 23 ];
    for( inst = 0; inst < 16; inst++ ) {

        micromod_get_string( inst, string );
        songInfo += QStringLiteral("%1 - %2").arg(inst, 2).arg(string, 22);
        micromod_get_string( inst + 16, string );
        songInfo += QStringLiteral("%1 - %2\n").arg(inst + 16, 2).arg(string, 22);
    }

    songInfo += QStringLiteral("Song Duration: %1 seconds.").arg( m_samplesRemaining / m_format.sampleRate() );
    return songInfo;
}
}

MusicWidget::MusicWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MusicWidget)
{
    ui->setupUi(this);

    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelCount(2);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QSysInfo::ByteOrder == QSysInfo::LittleEndian ? QAudioFormat::LittleEndian : QAudioFormat::BigEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qFatal("Raw audio format not supported by backend, cannot play audio.");
        return;
    }

    m_audioOutput = new QAudioOutput(format, this);
    connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));

    m_audioOutput->setNotifyInterval(100);
    connect( m_audioOutput, &QAudioOutput::notify, [=](){
        if( !ui->sliderSongPosition->isSliderDown() )
            ui->sliderSongPosition->setValue( ui->sliderSongPosition->value() + 100 );
    });

    m_micromod = new ItsASecretToEverybody::MicromodDevice(format);
}

MusicWidget::~MusicWidget()
{
    delete ui;
    delete m_audioOutput;
    delete m_micromod;
}

void MusicWidget::setMusicData(const QByteArray &data)
{
    ui->pushButtonPlayPause->setChecked(false);

    m_audioOutput->stop();
    m_micromod->close();
    m_micromod->open(data);

    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText( m_micromod->songInfo() );

    ui->sliderSongPosition->setMaximum( m_micromod->songDurationMilliseconds() );
    ui->sliderSongPosition->setValue(0);
}

void MusicWidget::handleStateChanged(QAudio::State newState)
{
    qDebug() << newState;
    switch (newState) {

    case QAudio::IdleState:
        // Finished playing (no more data)
        m_audioOutput->stop();
        ui->pushButtonPlayPause->setChecked(false);
        break;

    case QAudio::StoppedState:
        // Stopped for other reasons
        ui->pushButtonPlayPause->setChecked(false);

        if (m_audioOutput->error() != QAudio::NoError) {
            // Error handling
            qDebug() << m_audioOutput->error();
        }
        break;

    default:
        break;
    }
}

void MusicWidget::on_pushButtonPlayPause_toggled(bool checked)
{
    if(checked) {
        if(m_audioOutput->state() == QAudio::SuspendedState) {
            m_audioOutput->resume();
        } else {
            m_audioOutput->start(m_micromod);
        }

        ui->pushButtonPlayPause->setText("||");

    } else {
        m_audioOutput->suspend();
        ui->pushButtonPlayPause->setText(">");
    }
}

void MusicWidget::on_sliderSongPosition_valueChanged(int value)
{
    uint minutes, seconds, mseconds;

    minutes = value / ( 60 * 1000 );
    value -= minutes * ( 60 * 1000 );

    seconds = value / 1000;
    value -= seconds * 1000;

    mseconds = value / 100;

    ui->labelSongPosition->setText( QStringLiteral("%1m:%2.%3s")
                                    .arg( minutes, 2, 10, QChar('0') )
                                    .arg( seconds, 2, 10, QChar('0') )
                                    .arg( mseconds, 1, 10, QChar('0') )
                                    );
}

void MusicWidget::on_sliderSongPosition_sliderReleased()
{
    m_micromod->setSongPosition(ui->sliderSongPosition->value());
}

void MusicWidget::on_pushButtonStop_clicked()
{
    m_audioOutput->stop();
    m_micromod->rewind();
    ui->sliderSongPosition->setValue(0);
    ui->pushButtonPlayPause->setChecked(false);
}
