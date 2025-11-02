#pragma once
#include <JuceHeader.h>

class PlayerAudio
{
public:
    PlayerAudio();
    ~PlayerAudio();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    bool loadfile(const juce::File& file, juce::String& metadata);

    void play();
    void pause();
    bool isPlaying() const { return transportSource.isPlaying(); }

    void setGain(float gain);
    float getGain() const { return currentGain; }

    void mute();
    bool isMuted() const { return muted; }
    float getPreviousVolume() const { return previousVolume; }

    void setSpeed(float newSpeed);
    float getSpeed() const { return currentSpeed; }

    void setPosition(double pos);
    double getPosition() const;
    double getLength() const;

    void setlooping(bool shouldloop);
    bool islooping() const { return loopingEnabled; }

    void setLoopPoints(double startTime, double endTime);
    void clearLoopPoints();
    bool isSegmentLooping() const { return segmentLoopEnabled; }
    double getLoopStart() const { return loopStartTime; }
    double getLoopEnd() const { return loopEndTime; }

    void applyFadeIn();
    void applyFadeOut();
    void removeFades();
    bool hasFadeInApplied() const { return fadeInEnabled; }
    bool hasFadeOutApplied() const { return fadeOutEnabled; }

    void addMarker(double time, const juce::String& name = "");
    void removeMarker(int index);
    void removeAllMarkers();
    void jumpToMarker(int index);
    std::vector<std::pair<double, juce::String>> getMarkers() const { return markers; }
    int getNumMarkers() const { return static_cast<int>(markers.size()); }

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::ResamplingAudioSource resamplerSource;

    float previousVolume = 0.5f;
    float currentGain = 0.5f;
    float currentSpeed = 1.0f;
    bool muted = false;
    bool loopingEnabled = false;

    bool segmentLoopEnabled = false;
    double loopStartTime = 0.0;
    double loopEndTime = 0.0;
    double previousPosition = 0.0;

    bool fadeInEnabled = false;
    bool fadeOutEnabled = false;
    double fadeInDuration = 0.0;
    double fadeOutDuration = 0.0;

    std::vector<std::pair<double, juce::String>> markers;

    void checkLoopBoundaries();
    double calculateFadeGain(double currentTime) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};