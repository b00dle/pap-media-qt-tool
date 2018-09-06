#ifndef VOLUME_MAPPER_H
#define VOLUME_MAPPER_H

class VolumeMapper
{
public:
    VolumeMapper();

    static int logarithmicToLinear(int v);
    static int linearToLogarithmic(int v);
};

#endif // VOLUME_MAPPER_H
