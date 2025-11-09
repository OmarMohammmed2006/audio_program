#include "AudioMixer.h"

AudioMixer::AudioMixer() = default;

void AudioMixer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mixBuffer.setSize(2, samplesPerBlockExpected);
}

void AudioMixer::mixAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill,
                                PlayerAudio& player1, PlayerAudio& player2)
{
    if (!enabled)
        return;
    
    mixBuffer.clear();
    
    juce::AudioSourceChannelInfo track1Info(&mixBuffer, 0, bufferToFill.numSamples);
    juce::AudioSourceChannelInfo track2Info(bufferToFill.buffer, 
                                            bufferToFill.startSample, 
                                            bufferToFill.numSamples);
    
    player1.getNextAudioBlock(track1Info);
    player2.getNextAudioBlock(track2Info);
    
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        auto* outputData = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
        auto* track1Data = mixBuffer.getReadPointer(
            juce::jmin(channel, mixBuffer.getNumChannels() - 1), 0);
        
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            outputData[sample] = (track1Data[sample] * track1Volume) +
                                 (outputData[sample] * track2Volume);
        }
    }
}

void AudioMixer::setEnabled(bool shouldBeEnabled)
{
    enabled = shouldBeEnabled;
}

void AudioMixer::setTrack1Volume(float volume)
{
    track1Volume = juce::jlimit(0.0f, 1.0f, volume);
}

void AudioMixer::setTrack2Volume(float volume)
{
    track2Volume = juce::jlimit(0.0f, 1.0f, volume);
}