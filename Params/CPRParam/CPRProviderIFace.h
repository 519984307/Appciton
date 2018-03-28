#pragma once

class CPRProviderIFace
{
public:
    // Set minimum acceptable CPR compression depth.
    virtual void setMinimumDepth(int depth) = 0;

    // Set metronome rate.
    virtual void setMetronomeRate(int rate) = 0;

    // Set acceptable CPR compression rate range.
    virtual void setPressRateRange(int low, int high) = 0;

    // Start/Stop CPR.
    virtual void enableCPR(bool enable) = 0;

    // Start/Stop Metronome.
    virtual void enableMetronome(bool enable) = 0;

    // 构造与析构。
    CPRProviderIFace() { }
    virtual ~CPRProviderIFace() { }
};
