#include "volume_mapper.h"

#include <QAudio>

VolumeMapper::VolumeMapper()
{}

int VolumeMapper::logarithmicToLinear(int v)
{
    qreal linear_volume = QAudio::convertVolume(
        v / qreal(100.0),
        QAudio::LogarithmicVolumeScale,
        QAudio::LinearVolumeScale
    );
    return qRound(linear_volume*100);
}

int VolumeMapper::linearToLogarithmic(int v)
{
    qreal log_volume = QAudio::convertVolume(
        v / qreal(100.0),
        QAudio::LinearVolumeScale,
        QAudio::LogarithmicVolumeScale
    );
    return qRound(log_volume*100);
}
