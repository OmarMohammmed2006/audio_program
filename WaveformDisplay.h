#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class WaveformDisplay : public juce::Component
{
public:
    WaveformDisplay(juce::AudioThumbnail& thumb, PlayerAudio& player, const juce::String& trackName);
    
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    
    void setActive(bool shouldBeActive);
    void setMixerMode(bool isInMixerMode);
    bool isActive() const { return active; }
    
    std::function<void()> onWaveformClicked;

private:
    juce::AudioThumbnail& thumbnail;
    PlayerAudio& audioPlayer;
    juce::String name;
    
    bool active = false;
    bool mixerMode = false;
    bool isDragging = false;
    
    void drawWaveform(juce::Graphics& g);
    void drawPlayhead(juce::Graphics& g, juce::Rectangle<int> area);
    void drawMarkers(juce::Graphics& g, juce::Rectangle<int> area);
    void drawLoopRegion(juce::Graphics& g, juce::Rectangle<int> area);
    void drawTimeLabels(juce::Graphics& g, juce::Rectangle<int> area);
    
    double getTimeFromX(int xPosition, juce::Rectangle<int> area) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};