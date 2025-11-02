#pragma once
#include <JuceHeader.h>
#include "PlayerGUI.h"
#include "WaveformDisplay.h"
#include "SessionManager.h"
#include "AudioMixer.h"

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

private:
    PlayerAudio player1Audio;
    PlayerAudio player2Audio;
    PlayerAudio* activePlayer;

    PlayerGUI controls;
    std::unique_ptr<WaveformDisplay> waveform1;
    std::unique_ptr<WaveformDisplay> waveform2;
    juce::Label metadataLabel1;
    juce::Label metadataLabel2;

    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail1;
    juce::AudioThumbnail thumbnail2;

    SessionManager sessionManager;
    AudioMixer mixer;

    juce::Image backgroundImage;

    bool track1Active = true;
    juce::File lastLoadedFile1;
    juce::File lastLoadedFile2;
    int saveCounter = 0;

    void setupWaveformDisplays();
    void setupControls();
    void switchToTrack(int trackNumber);
    void handleFileLoaded(juce::File file);
    void handleMixerChanged(bool enabled, float vol1, float vol2);
    void handleMixerPlayPause();
    void updateMetadataDisplay(const juce::String& metadata, int trackNumber);
    void saveCurrentSession();
    void loadPreviousSession();
    void loadBackgroundImage();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};