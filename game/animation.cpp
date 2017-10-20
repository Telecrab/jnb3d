#include "animation.h"

#include <cmath>

Animation::Animation(float start, float end, const Seconds &duration, bool looping)
    : m_start(start)
    , m_end(end)
    , m_delta(m_end - m_start)
    , m_duration(duration)
    , m_elapsed(0)
    , m_isLooping(looping)
    , m_isPlaying(false)
{}

float Animation::start() const
{
    return m_start;
}

void Animation::setStart(float start)
{
    m_start = start;
}

float Animation::end() const
{
    return m_end;
}

void Animation::setEnd(float end)
{
    m_end = end;
}

Seconds Animation::duration() const
{
    return m_duration;
}

void Animation::setDuration(const Seconds &duration)
{
    if( duration < ElapsedType(1) ) return;
    m_duration = duration;
}

float Animation::calculateValue(const Seconds &time)
{
    float period = time / m_duration;
    float integral;
    float fractional = std::modf(period, &integral);

    return m_start + (fractional * m_delta);
}

float Animation::value(const Seconds &time)
{
    if( time <= Seconds::zero() ) return m_start;

    if(m_isLooping) {
        return calculateValue(time);
    } else {
        if (time >= m_duration) return m_end;
        else return calculateValue(time);
    }
}

float Animation::value()
{
    return value(m_elapsed);
}

void Animation::start()
{
    m_isPlaying = true;
}

void Animation::pause()
{
    m_isPlaying = false;
}

void Animation::stop()
{
    m_isPlaying = false;
    m_elapsed = ElapsedType::zero();
}

void Animation::update(const Seconds &delta)
{
    if(!m_isPlaying) return;

    m_elapsed += std::chrono::duration_cast<ElapsedType>(delta);
}
