#pragma once

#include <JuceHeader.h>
#include "PlayerGUI.h"

class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;
    
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources() override;
	void paint(juce::Graphics& g) override;
    void resized() override;

private:
    PlayerGUI player1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)

};