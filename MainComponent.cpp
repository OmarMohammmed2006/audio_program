#include "MainComponent.h"

MainComponent::MainComponent()
    : thumbnailCache(5),
    thumbnail1(512, formatManager, thumbnailCache),
    thumbnail2(512, formatManager, thumbnailCache),
    activePlayer(&player1Audio)
{
    formatManager.registerBasicFormats();

    addAndMakeVisible(metadataLabel1);
    addAndMakeVisible(metadataLabel2);

    for (auto* label : { &metadataLabel1, &metadataLabel2 })
    {
        label->setColour(juce::Label::textColourId, juce::Colours::white);
        label->setJustificationType(juce::Justification::topLeft);
        label->setFont(juce::Font(juce::FontOptions(12.0f)));
    }

    controls.connectToPlayer(activePlayer);

    controls.setOnFileLoadedCallback([this](juce::File file) {
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
        });

    addAndMakeVisible(controls);
    setSize(1000, 700);
    setAudioChannels(0, 2);
    startTimer(33);
    juce::Timer::callAfterDelay(500, [this]() {
        loadSession();
        });
}

MainComponent::~MainComponent()
{
    saveSession();
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1Audio.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2Audio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    if (track1Active) {
        player1Audio.getNextAudioBlock(bufferToFill);
    }
    else {
        player2Audio.getNextAudioBlock(bufferToFill);
    }
}

void MainComponent::releaseResources()
{
    player1Audio.releaseResources();
    player2Audio.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient(
        juce::Colours::cyan,
        0.0f, 0.0f,
        juce::Colour(juce::Colours::darkblue),
        0.0f, (float)getHeight(),
        false
    );
    g.setGradientFill(gradient);
    g.fillAll();

    auto area = getLocalBounds();
    auto waveformArea = area.removeFromTop(150);
    auto track1Area = waveformArea.removeFromLeft(getWidth() / 2).reduced(10, 5);
    auto track2Area = waveformArea.reduced(10, 5);

    // Define the drawWaveform lambda function
    auto drawWaveform = [&g](juce::AudioThumbnail& thumbnail, PlayerAudio* player, juce::Rectangle<int> area, bool isActive, const juce::String& name) {
        g.setColour(isActive ? juce::Colours::blue.withAlpha(0.4f) : juce::Colours::black.withAlpha(0.3f));
        g.fillRoundedRectangle(area.toFloat(), 8.0f);
        g.setColour(isActive ? juce::Colours::yellow : juce::Colours::white.withAlpha(0.5f));
        g.drawRoundedRectangle(area.toFloat(), 8.0f, 2.0f);

        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(juce::FontOptions(14.0f)));
        g.drawText(name, area.removeFromTop(20), juce::Justification::centred);

        if (thumbnail.getTotalLength() > 0.0)
        {
            auto drawArea = area.reduced(5);

            g.setColour(isActive ? juce::Colours::cyan : juce::Colours::lightblue.withAlpha(0.7f));
            thumbnail.drawChannel(g, drawArea, 0.0, thumbnail.getTotalLength(), 0, 1.0f);

            auto currentPos = player->getPosition();
            if (thumbnail.getTotalLength() > 0.0)
            {
                auto playheadX = drawArea.getX() + (currentPos / thumbnail.getTotalLength()) * drawArea.getWidth();
                g.setColour(juce::Colours::red);
                g.drawLine((float)playheadX, (float)drawArea.getY(), (float)playheadX, (float)drawArea.getBottom(), 2.0f);
            }

            g.setColour(juce::Colours::white);
            g.setFont(juce::Font(juce::FontOptions(12.0f)));

            auto formatTime = [](double seconds) {
                if (seconds < 0.0) seconds = 0.0;
                int mins = static_cast<int>(seconds) / 60;
                int secs = static_cast<int>(seconds) % 60;
                return juce::String::formatted("%d:%02d", mins, secs);
                };

            g.drawText(formatTime(currentPos), drawArea, juce::Justification::bottomLeft);
            g.drawText(formatTime(thumbnail.getTotalLength()), drawArea, juce::Justification::bottomRight);
        }
        else
        {
            g.setColour(juce::Colours::white.withAlpha(0.7f));
            g.drawText("Click to load audio file", area, juce::Justification::centred);
        }
        };

    // Draw the waveforms
    drawWaveform(thumbnail1, &player1Audio, track1Area, track1Active, "Track 1");
    drawWaveform(thumbnail2, &player2Audio, track2Area, track2Active, "Track 2");
}

void MainComponent::resized()
{
    auto area = getLocalBounds();

    auto waveformArea = area.removeFromTop(150);

    auto metadataArea = area.removeFromTop(80); // Space for metadata
    auto track1MetadataArea = metadataArea.removeFromLeft(getWidth() / 2).reduced(10, 5);
    auto track2MetadataArea = metadataArea.reduced(10, 5);

    metadataLabel1.setBounds(track1MetadataArea);
    metadataLabel2.setBounds(track2MetadataArea);

    controls.setBounds(area);
}

void MainComponent::timerCallback()
{
    repaint();
    static int saveCounter = 0;
    if (++saveCounter >= 150)
    {
        saveSession();
        saveCounter = 0;
    }
}

void MainComponent::mouseDown(const juce::MouseEvent& event)
{
    auto area = getLocalBounds();
    auto waveformArea = area.removeFromTop(150);
    auto track1Area = waveformArea.removeFromLeft(getWidth() / 2).reduced(10, 5);
    auto track2Area = waveformArea.reduced(10, 5);

    if (track2Area.contains(event.getPosition()))
    {
        bool wasPlayingBeforeSwitch = track2Active ? player2Audio.isPlaying() : player1Audio.isPlaying();
        if (!track2Active)
        {
            if (player1Audio.isPlaying())
            {
                player1Audio.pause();
            }
            track1Active = false;
            track2Active = true;
            activePlayer = &player2Audio;
            controls.connectToPlayer(activePlayer);
        }

        if (thumbnail2.getTotalLength() > 0.0)
        {
            auto drawArea = track2Area.reduced(5);
            double clickPos = (event.getPosition().x - drawArea.getX()) / (double)drawArea.getWidth();
            clickPos = juce::jlimit(0.0, 1.0, clickPos);
            double newTime = clickPos * thumbnail2.getTotalLength();
            player2Audio.setPosition(newTime);
            if (event.mods.isRightButtonDown()) {
                player2Audio.setLoopPoints(newTime, player2Audio.getLoopEnd());
            }
            else if (event.mods.isMiddleButtonDown()) {
                player2Audio.setLoopPoints(player2Audio.getLoopStart(), newTime);
            }

            if (wasPlayingBeforeSwitch)
            {
                player2Audio.play();
            }
            isDraggingPlayhead = true;
            activeTrackDragging = 2;
        }
        repaint();
    }
    else if (track1Area.contains(event.getPosition()))
    {
        bool wasPlayingBeforeSwitch = track1Active ? player1Audio.isPlaying() : player2Audio.isPlaying();

        if (!track1Active)
        {
            if (player2Audio.isPlaying())
            {
                player2Audio.pause();
            }
            track1Active = true;
            track2Active = false;
            activePlayer = &player1Audio;
            controls.connectToPlayer(activePlayer);
        }

        if (thumbnail1.getTotalLength() > 0.0)
        {
            auto drawArea = track1Area.reduced(5);
            double clickPos = (event.getPosition().x - drawArea.getX()) / (double)drawArea.getWidth();
            clickPos = juce::jlimit(0.0, 1.0, clickPos);
            double newTime = clickPos * thumbnail1.getTotalLength();
            player1Audio.setPosition(newTime);
            if (event.mods.isRightButtonDown()) {
                player1Audio.setLoopPoints(newTime, player1Audio.getLoopEnd());
            }
            else if (event.mods.isMiddleButtonDown()) {
                player1Audio.setLoopPoints(player1Audio.getLoopStart(), newTime);
            }
            if (wasPlayingBeforeSwitch)
            {
                player1Audio.play();
            }
            isDraggingPlayhead = true;
            activeTrackDragging = 1;
        }
        repaint();
    }
    saveSession();
}

void MainComponent::mouseDrag(const juce::MouseEvent& event)
{
    if (isDraggingPlayhead)
    {
        auto area = getLocalBounds();
        auto waveformArea = area.removeFromTop(150);

        if (activeTrackDragging == 1)
        {
            auto track1Area = waveformArea.removeFromLeft(getWidth() / 2).reduced(10, 5);
            auto drawArea = track1Area.reduced(5);

            if (thumbnail1.getTotalLength() > 0.0) {
                double dragPos = (event.getPosition().x - drawArea.getX()) / (double)drawArea.getWidth();
                dragPos = juce::jlimit(0.0, 1.0, dragPos);
                double newTime = dragPos * thumbnail1.getTotalLength();
                player1Audio.setPosition(newTime);
                repaint();
            }
        }
        else if (activeTrackDragging == 2)
        {
            auto track2Area = waveformArea.reduced(10, 5);
            auto drawArea = track2Area.reduced(5);

            if (thumbnail2.getTotalLength() > 0.0) {
                double dragPos = (event.getPosition().x - drawArea.getX()) / (double)drawArea.getWidth();
                dragPos = juce::jlimit(0.0, 1.0, dragPos);
                double newTime = dragPos * thumbnail2.getTotalLength();
                player2Audio.setPosition(newTime);
                repaint();
            }
        }
    }
}

void MainComponent::mouseUp(const juce::MouseEvent& event)
{
    isDraggingPlayhead = false;
    activeTrackDragging = 0;
    saveSession();
}

void MainComponent::updateMetadataDisplay(const juce::String& metadata, int trackNumber)
{
    if (trackNumber == 1)
    {
        metadataLabel1.setText(metadata, juce::dontSendNotification);
    }
    else if (trackNumber == 2)
    {
        metadataLabel2.setText(metadata, juce::dontSendNotification);
    }
    repaint();
}

void MainComponent::drawLoopRegion(juce::Graphics& g, juce::AudioThumbnail& thumbnail, juce::Rectangle<int> area, PlayerAudio* player)
{
    if (player->isSegmentLooping() && thumbnail.getTotalLength() > 0.0)
    {
        auto drawArea = area.reduced(5);
        double loopStart = player->getLoopStart();
        double loopEnd = player->getLoopEnd();
        double totalLength = thumbnail.getTotalLength();

        // Calculate loop region bounds
        float startX = (float)(drawArea.getX() + (loopStart / totalLength) * drawArea.getWidth());
        float endX = (float)(drawArea.getX() + (loopEnd / totalLength) * drawArea.getWidth());

        // Draw loop region highlight
        g.setColour(juce::Colours::green.withAlpha(0.2f));
        g.fillRect(startX, (float)drawArea.getY(), endX - startX, (float)drawArea.getHeight());

        // Draw loop boundary lines
        g.setColour(juce::Colours::green);
        g.drawLine(startX, (float)drawArea.getY(), startX, (float)drawArea.getBottom(), 2.0f);
        g.drawLine(endX, (float)drawArea.getY(), endX, (float)drawArea.getBottom(), 2.0f);

        // Draw loop labels
        g.setFont(juce::Font(juce::FontOptions(10.0f, juce::Font::bold)));
        g.drawText("A", (int)(startX - 10), drawArea.getY() - 15, 20, 15, juce::Justification::centred);
        g.drawText("B", (int)(endX - 10), drawArea.getY() - 15, 20, 15, juce::Justification::centred);
    }
}

juce::PropertiesFile* MainComponent::getSettingsFile()
{
    juce::PropertiesFile::Options options;
    options.applicationName = "AudioPlayer";
    options.filenameSuffix = "settings";
    options.osxLibrarySubFolder = "Application Support";
    juce::File settingsFolder = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("AudioPlayer");
    options.folderName = settingsFolder.getFullPathName();

    return new juce::PropertiesFile(options);
}

void MainComponent::saveSession()
{
    std::unique_ptr<juce::PropertiesFile> settings(getSettingsFile());
    lastPosition1 = player1Audio.getPosition();
    lastPosition2 = player2Audio.getPosition();
    wasPlaying1 = player1Audio.isPlaying();
    wasPlaying2 = player2Audio.isPlaying();
    lastActiveTrack = track1Active ? 1 : 2;
    settings->setValue("lastFile1", lastLoadedFile1.getFullPathName());
    settings->setValue("lastFile2", lastLoadedFile2.getFullPathName());
    settings->setValue("lastPosition1", lastPosition1);
    settings->setValue("lastPosition2", lastPosition2);
    settings->setValue("wasPlaying1", wasPlaying1);
    settings->setValue("wasPlaying2", wasPlaying2);
    settings->setValue("lastActiveTrack", lastActiveTrack);
    settings->save();
}

void MainComponent::loadSession()
{
    std::unique_ptr<juce::PropertiesFile> settings(getSettingsFile());

    if (settings->getFile().existsAsFile())
    {
        juce::String filePath1 = settings->getValue("lastFile1");
        juce::String filePath2 = settings->getValue("lastFile2");

        lastPosition1 = settings->getDoubleValue("lastPosition1", 0.0);
        lastPosition2 = settings->getDoubleValue("lastPosition2", 0.0);
        wasPlaying1 = settings->getBoolValue("wasPlaying1", false);
        wasPlaying2 = settings->getBoolValue("wasPlaying2", false);
        lastActiveTrack = settings->getIntValue("lastActiveTrack", 1);

        if (juce::File::isAbsolutePath(filePath1))
        {
            juce::File file1(filePath1);
            if (file1.existsAsFile())
            {
                juce::String metadata;
                if (player1Audio.loadfile(file1, metadata))
                {
                    thumbnail1.setSource(new juce::FileInputSource(file1));
                    updateMetadataDisplay(metadata, 1);
                    lastLoadedFile1 = file1;

                    player1Audio.setPosition(lastPosition1);

                    if (wasPlaying1)
                    {
                        player1Audio.play();
                    }
                }
            }
        }

        if (juce::File::isAbsolutePath(filePath2))
        {
            juce::File file2(filePath2);
            if (file2.existsAsFile())
            {
                juce::String metadata;
                if (player2Audio.loadfile(file2, metadata))
                {
                    thumbnail2.setSource(new juce::FileInputSource(file2));
                    updateMetadataDisplay(metadata, 2);
                    lastLoadedFile2 = file2;

                    player2Audio.setPosition(lastPosition2);

                    // Restore play state if it was playing
                    if (wasPlaying2)
                    {
                        player2Audio.play();
                    }
                }
            }
        }

        if (lastActiveTrack == 1 && track1Active != true)
        {
            track1Active = true;
            track2Active = false;
            activePlayer = &player1Audio;
            controls.connectToPlayer(activePlayer);
        }
        else if (lastActiveTrack == 2 && track2Active != true)
        {
            track1Active = false;
            track2Active = true;
            activePlayer = &player2Audio;
            controls.connectToPlayer(activePlayer);
        }

        repaint();
    }
}

void MainComponent::loadPlaylistFile(const juce::File& file)
{
    juce::String metadata;
    if (track1Active)
    {
        if (player1Audio.loadfile(file, metadata))
        {
            thumbnail1.setSource(new juce::FileInputSource(file));
            updateMetadataDisplay(metadata, 1);
            lastLoadedFile1 = file;
            activePlayer = &player1Audio;
        }
    }
    else
    {
        if (player2Audio.loadfile(file, metadata))
        {
            thumbnail2.setSource(new juce::FileInputSource(file));
            updateMetadataDisplay(metadata, 2);
            lastLoadedFile2 = file;
            activePlayer = &player2Audio;
        }
    }
    repaint();
}

void MainComponent::playNextInPlaylist()
{
    if (playlist.isEmpty() || currentPlaylistIndex < 0) return;
    currentPlaylistIndex = (currentPlaylistIndex + 1) % playlist.size();
    loadPlaylistFile(playlist[currentPlaylistIndex]);
    activePlayer->play();
}

void MainComponent::playPreviousInPlaylist()
{
    if (playlist.isEmpty() || currentPlaylistIndex < 0) return;
    currentPlaylistIndex = (currentPlaylistIndex - 1 + playlist.size()) % playlist.size();
    loadPlaylistFile(playlist[currentPlaylistIndex]);
    activePlayer->play();
}