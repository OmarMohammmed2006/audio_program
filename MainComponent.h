#pragma once

#include <JuceHeader.h>
#include "PlayerGUI.h"

class MainComponent : public juce::AudioAppComponent, public juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

private:
    PlayerAudio player1Audio;
    PlayerAudio player2Audio;
    PlayerGUI controls;

    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail1;
    juce::AudioThumbnail thumbnail2;
    juce::Label metadataLabel1;
    juce::Label metadataLabel2;

    PlayerAudio* activePlayer;
    bool track1Active = true;
    bool track2Active = false;
    bool isDraggingPlayhead = false;
    int activeTrackDragging = 0;

    void updateMetadataDisplay(const juce::String& metadata, int trackNumber);
    void drawLoopRegion(juce::Graphics& g, juce::AudioThumbnail& thumbnail, juce::Rectangle<int> area, PlayerAudio* player);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};