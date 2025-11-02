#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class AudioMixer
{
public:
    AudioMixer();
    
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void mixAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill,
                       PlayerAudio& player1, PlayerAudio& player2);
    
    void setEnabled(bool shouldBeEnabled);
    bool isEnabled() const { return enabled; }
    
    void setTrack1Volume(float volume);
    void setTrack2Volume(float volume);
    
    float getTrack1Volume() const { return track1Volume; }
    float getTrack2Volume() const { return track2Volume; }
    
private:
    bool enabled = false;
    float track1Volume = 0.7f;
    float track2Volume = 0.7f;
    juce::AudioBuffer<float> mixBuffer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioMixer)
};