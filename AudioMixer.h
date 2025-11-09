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
    
    void setEnabled(bool shouldBeEnabled); // updates the state of the audio mixer later
    bool isEnabled() const { return enabled; } // it's a setter function to set mode to enabled variable
    
    void setTrack1Volume(float volume);
    void setTrack2Volume(float volume);
    
    float getTrack1Volume() const { return track1Volume; } // to get the percentage of the volume when mixer is on for track 1
    float getTrack2Volume() const { return track2Volume; } // for track 2
    
private:
    bool enabled = false;
    float track1Volume = 0.7f;
    float track2Volume = 0.7f;
    juce::AudioBuffer<float> mixBuffer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioMixer)
};