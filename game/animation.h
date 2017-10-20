#ifndef ANIMATION_H
#define ANIMATION_H

#include "common.h"

class Animation
{
public:
    Animation(float start = 1.0f, float end = 1.0f, const Seconds &duration = Seconds(1.0f), bool looping = false);

    float start() const;
    void setStart(float start);

    float end() const;
    void setEnd(float end);

    Seconds duration() const;
    void setDuration(const Seconds &duration);

    float value();

    void start();
    void pause();
    void stop();

    void update(const Seconds &delta);

private:
    using ElapsedType = Microseconds;

    float calculateValue(const Seconds &time);
    float value(const Seconds &time);

    float m_start;
    float m_end;
    float m_delta;
    Seconds m_duration;
    ElapsedType m_elapsed;
    bool m_isLooping;
    bool m_isPlaying;
};

#endif // ANIMATION_H
