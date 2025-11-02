#include "MainComponent.h"
#include <BinaryData.h>

MainComponent::MainComponent()
    : thumbnailCache(5),
      thumbnail1(512, formatManager, thumbnailCache),
      thumbnail2(512, formatManager, thumbnailCache),
      activePlayer(&player1Audio)
{
    formatManager.registerBasicFormats();

    loadBackgroundImage();

    setupWaveformDisplays();
    setupControls();

    addAndMakeVisible(metadataLabel1);
    addAndMakeVisible(metadataLabel2);

    for (auto* label : { &metadataLabel1, &metadataLabel2 })
    {
        label->setColour(juce::Label::textColourId, juce::Colours::white);
        label->setJustificationType(juce::Justification::topLeft);
        label->setFont(juce::Font(12.0f));
    }

    addAndMakeVisible(controls);
    setSize(1000, 700);
    setAudioChannels(0, 2);
    startTimer(33);

    juce::Timer::callAfterDelay(500, [this]() { loadPreviousSession(); });
}

void MainComponent::loadBackgroundImage()
{
    int dataSize = 0;
    const char* imageData = BinaryData::getNamedResource("bg2_jpg", dataSize);

    if (imageData != nullptr && dataSize > 0)
    {
        backgroundImage = juce::ImageCache::getFromMemory(imageData, dataSize);
    }
}

void MainComponent::setupWaveformDisplays()
{
    waveform1 = std::make_unique<WaveformDisplay>(thumbnail1, player1Audio, "Track 1");
    waveform2 = std::make_unique<WaveformDisplay>(thumbnail2, player2Audio, "Track 2");

    waveform1->setActive(true);

    waveform1->onWaveformClicked = [this]() {
        if (!mixer.isEnabled())
        {
            switchToTrack(1);
        }
    };

    waveform2->onWaveformClicked = [this]() {
        if (!mixer.isEnabled())
        {
            switchToTrack(2);
        }
    };

    addAndMakeVisible(waveform1.get());
    addAndMakeVisible(waveform2.get());
}

void MainComponent::setupControls()
{
    controls.connectToPlayer(activePlayer);

    controls.setMixerCallback([this](bool enabled, float vol1, float vol2) {
        handleMixerChanged(enabled, vol1, vol2);
    });

    controls.setMixerPlayPauseCallback([this]() {
        handleMixerPlayPause();
    });

    controls.setOnFileLoadedCallback([this](juce::File file) {
        handleFileLoaded(file);
    });
}

void MainComponent::handleFileLoaded(juce::File file)
{
    juce::String metadata;

    if (track1Active)
    {
        thumbnail1.setSource(new juce::FileInputSource(file));
        if (player1Audio.loadfile(file, metadata))
        {
            updateMetadataDisplay(metadata, 1);
            lastLoadedFile1 = file;
        }
    }
    else
    {
        thumbnail2.setSource(new juce::FileInputSource(file));
        if (player2Audio.loadfile(file, metadata))
        {
            updateMetadataDisplay(metadata, 2);
            lastLoadedFile2 = file;
        }
    }
}

void MainComponent::handleMixerChanged(bool enabled, float vol1, float vol2)
{
    mixer.setEnabled(enabled);
    mixer.setTrack1Volume(vol1);
    mixer.setTrack2Volume(vol2);

    waveform1->setMixerMode(enabled);
    waveform2->setMixerMode(enabled);
}

void MainComponent::handleMixerPlayPause()
{
    bool anyPlaying = player1Audio.isPlaying() || player2Audio.isPlaying();

    if (anyPlaying)
    {
        player1Audio.pause();
        player2Audio.pause();
    }
    else
    {
        player1Audio.play();
        player2Audio.play();
    }
}

void MainComponent::switchToTrack(int trackNumber)
{
    if (trackNumber == 1 && !track1Active)
    {
        bool wasPlaying = player2Audio.isPlaying();

        if (player2Audio.isPlaying())
            player2Audio.pause();

        track1Active = true;
        activePlayer = &player1Audio;
        controls.connectToPlayer(activePlayer);

        waveform1->setActive(true);
        waveform2->setActive(false);

        if (wasPlaying)
            player1Audio.play();

        repaint();
    }
    else if (trackNumber == 2 && track1Active)
    {
        bool wasPlaying = player1Audio.isPlaying();

        if (player1Audio.isPlaying())
            player1Audio.pause();

        track1Active = false;
        activePlayer = &player2Audio;
        controls.connectToPlayer(activePlayer);

        waveform1->setActive(false);
        waveform2->setActive(true);

        if (wasPlaying)
            player2Audio.play();

        repaint();
    }
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1Audio.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2Audio.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    if (mixer.isEnabled())
    {
        mixer.mixAudioBlock(bufferToFill, player1Audio, player2Audio);
    }
    else
    {
        if (track1Active)
            player1Audio.getNextAudioBlock(bufferToFill);
        else
            player2Audio.getNextAudioBlock(bufferToFill);
    }
}

void MainComponent::paint(juce::Graphics& g)
{
    if (backgroundImage.isValid())
    {
        g.drawImage(backgroundImage, getLocalBounds().toFloat(),
                   juce::RectanglePlacement::fillDestination);
    }
    else
    {
        juce::ColourGradient gradient(
            juce::Colours::cyan, 0, 0,
            juce::Colour(juce::Colours::darkblue), 0, getHeight(),
            false
        );
        g.setGradientFill(gradient);
        g.fillAll();
    }
}

void MainComponent::resized()
{
    auto area = getLocalBounds();

    auto waveformArea = area.removeFromTop(150);
    auto track1Area = waveformArea.removeFromLeft(getWidth() / 2).reduced(10, 5);
    auto track2Area = waveformArea.reduced(10, 5);

    waveform1->setBounds(track1Area);
    waveform2->setBounds(track2Area);

    auto metadataArea = area.removeFromTop(80);
    auto track1MetadataArea = metadataArea.removeFromLeft(getWidth() / 2).reduced(10, 5);
    auto track2MetadataArea = metadataArea.reduced(10, 5);

    metadataLabel1.setBounds(track1MetadataArea);
    metadataLabel2.setBounds(track2MetadataArea);

    controls.setBounds(area);
}

void MainComponent::timerCallback()
{
    repaint();

    if (++saveCounter >= 150)
    {
        saveCurrentSession();
        saveCounter = 0;
    }
}

void MainComponent::saveCurrentSession()
{
    SessionData data;
    data.file1 = lastLoadedFile1;
    data.file2 = lastLoadedFile2;
    data.position1 = player1Audio.getPosition();
    data.position2 = player2Audio.getPosition();
    data.wasPlaying1 = player1Audio.isPlaying();
    data.wasPlaying2 = player2Audio.isPlaying();
    data.activeTrack = track1Active ? 1 : 2;

    sessionManager.saveSession(data);
}

void MainComponent::loadPreviousSession()
{
    SessionData data = sessionManager.loadSession();

    if (data.file1.existsAsFile())
    {
        juce::String metadata;
        if (player1Audio.loadfile(data.file1, metadata))
        {
            thumbnail1.setSource(new juce::FileInputSource(data.file1));
            updateMetadataDisplay(metadata, 1);
            lastLoadedFile1 = data.file1;
            player1Audio.setPosition(data.position1);

            if (data.wasPlaying1)
                player1Audio.play();
        }
    }

    if (data.file2.existsAsFile())
    {
        juce::String metadata;
        if (player2Audio.loadfile(data.file2, metadata))
        {
            thumbnail2.setSource(new juce::FileInputSource(data.file2));
            updateMetadataDisplay(metadata, 2);
            lastLoadedFile2 = data.file2;
            player2Audio.setPosition(data.position2);

            if (data.wasPlaying2)
                player2Audio.play();
        }
    }

    switchToTrack(data.activeTrack);
    repaint();
}

void MainComponent::updateMetadataDisplay(const juce::String& metadata, int trackNumber)
{
    if (trackNumber == 1)
        metadataLabel1.setText(metadata, juce::dontSendNotification);
    else if (trackNumber == 2)
        metadataLabel2.setText(metadata, juce::dontSendNotification);

    repaint();
}

MainComponent::~MainComponent()
{
    saveCurrentSession();
    shutdownAudio();
}

void MainComponent::releaseResources()
{
    player1Audio.releaseResources();
    player2Audio.releaseResources();
}