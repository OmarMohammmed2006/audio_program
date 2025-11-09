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

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override; // initialize audio system
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override; // calls back the audio
    void releaseResources() override; // releases the audio

    void paint(juce::Graphics& g) override; // paints the gui components
    void resized() override;
    void timerCallback() override; // triggers the painting and auto-save after a certain time

private:
    PlayerAudio player1Audio; // creating an object 1 for track 1
    PlayerAudio player2Audio; // creating an object 2 for track 2
    PlayerAudio* activePlayer; // a pointer to dynamically switch between tracks

    PlayerGUI controls; // the main gui control panel
    std::unique_ptr<WaveformDisplay> waveform1; // this is the visual display of track 1 as waveform
    std::unique_ptr<WaveformDisplay> waveform2; // and this is the visual display of track 2 as waveform
    /* we used a unique pointer because we don't have the args for the waveformdisplay class so we use
     the pointer until we are ready to pass the args*/

    juce::Label metadataLabel1;
    juce::Label metadataLabel2;

    juce::AudioFormatManager formatManager; // manages the format of the audio uploaded
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail1;
    juce::AudioThumbnail thumbnail2;

    SessionManager sessionManager;
    // making an object of the class of session manager we created to manage our session memory

    AudioMixer mixer;
    // making an object of our mixer class

    juce::Image backgroundImage;
    // the background image of the program later used in maincomponent.cpp

    bool track1Active = true;
    // boolean to keep track of which track is currently active

    juce::File lastLoadedFile1;
    juce::File lastLoadedFile2;
    // Keeps track of the last loaded file in both tracks

    int saveCounter = 0;
    // is part of the auto save mechanism

    void setupWaveformDisplays();
    void setupControls();
    // setter functions to set up the components

    void switchToTrack(int trackNumber);
    // changes the active track to the other

    void handleFileLoaded(juce::File file);
    void handleMixerChanged(bool enabled, float vol1, float vol2);
    void handleMixerPlayPause();

    void updateMetadataDisplay(const juce::String& metadata, int trackNumber);
    // getter function to show the metadata of the loaded track

    void saveCurrentSession();
    void loadPreviousSession();
    // keeps track of sessions

    void loadBackgroundImage();
    // loads the custom background image

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};