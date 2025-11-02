#include "PlayerGUI.h"

void ModernLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
    const juce::Colour& backgroundColour,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
    const float cornerSize = 12.0f;

    juce::Colour baseColour = backgroundColour;

    if (shouldDrawButtonAsDown)
        baseColour = juce::Colours::white.withAlpha(0.4f);
    else if (shouldDrawButtonAsHighlighted)
        baseColour = juce::Colours::white.withAlpha(0.2f);

    juce::ColourGradient gradient(
        baseColour.brighter(shouldDrawButtonAsHighlighted ? 0.4f : 0.1f),
        bounds.getX(), bounds.getY(),
        baseColour.darker(0.1f),
        bounds.getX(), bounds.getBottom(),
        false
    );

    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, cornerSize);

    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.drawRoundedRectangle(bounds, cornerSize, 1.5f);
}

void ModernLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos, float minSliderPos, float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    if (style == juce::Slider::LinearHorizontal)
    {
        auto trackHeight = 6.0f;
        auto trackBounds = slider.getLocalBounds().toFloat().reduced(1.0f);
        auto trackY = trackBounds.getCentreY() - trackHeight * 0.5f;

        g.setColour(juce::Colours::black.withAlpha(0.4f));
        g.fillRoundedRectangle(trackBounds.getX(), trackY, trackBounds.getWidth(), trackHeight, trackHeight * 0.5f);

        if (slider.isEnabled())
        {
            g.setColour(juce::Colours::white.withAlpha(0.8f));
            g.fillRoundedRectangle(trackBounds.getX(), trackY, sliderPos - trackBounds.getX(), trackHeight, trackHeight * 0.5f);
        }

        auto thumbSize = 20.0f;
        juce::Point<float> thumbPoint(sliderPos, trackBounds.getCentreY());

        juce::ColourGradient thumbGradient(
            juce::Colours::white.withAlpha(0.9f),
            thumbPoint.x - thumbSize * 0.5f, thumbPoint.y - thumbSize * 0.5f,
            juce::Colours::white.withAlpha(0.6f),
            thumbPoint.x + thumbSize * 0.5f, thumbPoint.y + thumbSize * 0.5f,
            false
        );

        g.setGradientFill(thumbGradient);
        g.fillEllipse(juce::Rectangle<float>(thumbSize, thumbSize).withCentre(thumbPoint));

        g.setColour(juce::Colours::white.withAlpha(0.8f));
        g.drawEllipse(juce::Rectangle<float>(thumbSize, thumbSize).withCentre(thumbPoint), 1.5f);
    }
    else
    {
        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }
}

juce::Font ModernLookAndFeel::getTextButtonFont(juce::TextButton&, int buttonHeight)
{
    return juce::Font(14.0f, juce::Font::bold);
}

PlayerGUI::PlayerGUI()
{
    playlist = std::make_unique<PlaylistComponent>();
    addAndMakeVisible(playlist.get());

    playlist->onTrackLoadRequested = [this](juce::File file) {
        if (onFileLoaded)
            onFileLoaded(file);
    };

    setupButtons();
    setupSliders();
    setupLabels();

    markersList.setModel(this);
    markersList.setColour(juce::ListBox::backgroundColourId, juce::Colours::transparentBlack);
    markersList.setColour(juce::ListBox::outlineColourId, juce::Colours::white.withAlpha(0.5f));
    markersList.setOutlineThickness(1);
    markersList.setRowHeight(25);
    addAndMakeVisible(markersList);

    updateMixerControls();
}

void PlayerGUI::setupButtons()
{
    auto setupModernButton = [this](juce::TextButton& button) {
        button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
        button.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
        button.setColour(juce::TextButton::buttonOnColourId, juce::Colours::white.withAlpha(0.3f));
        button.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        button.setColour(juce::TextButton::textColourOffId, juce::Colours::white.withAlpha(0.9f));
        button.setLookAndFeel(&customLookAndFeel);
        addAndMakeVisible(&button);
        button.addListener(this);
    };

    for (auto* btn : { &loadButton, &restartButton, &playpauseButton,
        &skipBackButton, &skipForwardButton, &mute_button, &loopbutton,
        &gotostartbutton, &gotoendbutton, &speedHalfButton, &speedNormalButton,
        &speedDoubleButton, &speedQuadButton, &setPointAButton, &setPointBButton,
        &clearLoopPointsButton, &toggleSegmentLoopButton, &fadeInButton,
        &fadeOutButton, &removeFadesButton, &addMarkerButton, &clearMarkersButton,
        &mixerToggleButton })
    {
        setupModernButton(*btn);
    }
}

void PlayerGUI::setupSliders()
{
    auto setupSlider = [this](juce::Slider& slider, double min, double max, double initial) {
        slider.setRange(min, max, 0.01);
        slider.setValue(initial);
        slider.addListener(this);
        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setLookAndFeel(&customLookAndFeel);
        addAndMakeVisible(&slider);
    };

    setupSlider(volumeSlider, 0.0, 1.0, 0.5);
    setupSlider(speedSlider, 0.25, 4.0, 1.0);
    setupSlider(track1MixSlider, 0.0, 1.0, 0.7);
    setupSlider(track2MixSlider, 0.0, 1.0, 0.7);
}

void PlayerGUI::setupLabels()
{
    auto setupLabel = [this](juce::Label& label, const juce::String& text, juce::Colour colour) {
        label.setText(text, juce::dontSendNotification);
        label.setColour(juce::Label::textColourId, colour);
        label.setJustificationType(juce::Justification::centredLeft);
        label.setFont(juce::Font(12.0f, juce::Font::bold));
        addAndMakeVisible(&label);
    };

    setupLabel(volumeLabel, "Volume: 50%", juce::Colours::white.withAlpha(0.9f));
    setupLabel(speedLabel, "Speed: 1.0x", juce::Colours::white.withAlpha(0.9f));
    setupLabel(fadeStatusLabel, "Fades: None", juce::Colours::white.withAlpha(0.9f));
    setupLabel(loopStartLabel, "A: --:--", juce::Colours::yellow.withAlpha(0.9f));
    setupLabel(loopEndLabel, "B: --:--", juce::Colours::yellow.withAlpha(0.9f));
    setupLabel(track1MixLabel, "Track 1: 70%", juce::Colours::white.withAlpha(0.9f));
    setupLabel(track2MixLabel, "Track 2: 70%", juce::Colours::white.withAlpha(0.9f));

    loopEndLabel.setJustificationType(juce::Justification::centredRight);
}

PlayerGUI::~PlayerGUI()
{
    markersList.setModel(nullptr);

    for (auto* btn : { &loadButton, &restartButton, &playpauseButton,
        &skipBackButton, &skipForwardButton, &mute_button, &loopbutton,
        &gotostartbutton, &gotoendbutton, &speedHalfButton, &speedNormalButton,
        &speedDoubleButton, &speedQuadButton, &addMarkerButton, &clearMarkersButton,
        &mixerToggleButton })
    {
        btn->setLookAndFeel(nullptr);
    }

    volumeSlider.setLookAndFeel(nullptr);
    speedSlider.setLookAndFeel(nullptr);
    track1MixSlider.setLookAndFeel(nullptr);
    track2MixSlider.setLookAndFeel(nullptr);

    playlist.reset();
}

void PlayerGUI::paint(juce::Graphics& g)
{
    auto area = getLocalBounds();

    const int buttonHeight = 35;
    const int rowSpacing = 10;
    const int sliderHeight = 30;
    const int labelHeight = 20;
    const int markersListHeight = 120;
    const int mixerHeight = mixerEnabled ? (2 * labelHeight + 2 * sliderHeight + rowSpacing) : 0;

    int leftBoxHeight = (4 * buttonHeight) + (3 * rowSpacing) + markersListHeight;
    int rightBoxHeight = (2 * labelHeight) + (2 * sliderHeight) + (2 * buttonHeight) + (3 * rowSpacing) + 20 + mixerHeight;

    int controlAreaHeight = juce::jmax(leftBoxHeight, rightBoxHeight);
    auto controlArea = area.removeFromTop(controlAreaHeight + 20);
    auto playlistArea = area.reduced(10);

    auto leftBox = controlArea.removeFromLeft(getWidth() / 2).reduced(5);
    auto rightBox = controlArea.reduced(5);

    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.drawRoundedRectangle(leftBox.toFloat(), 8.0f, 2.0f);
    g.drawRoundedRectangle(rightBox.toFloat(), 8.0f, 2.0f);

    if (!area.isEmpty())
    {
        g.drawRoundedRectangle(area.toFloat(), 8.0f, 2.0f);
    }
}

void PlayerGUI::resized()
{
    auto area = getLocalBounds();

    const int buttonHeight = 35;
    const int rowSpacing = 10;
    const int sliderHeight = 30;
    const int labelHeight = 20;
    const int markersListHeight = 120;
    const int mixerHeight = mixerEnabled ? (2 * labelHeight + 2 * sliderHeight + rowSpacing) : 0;

    int leftBoxHeight = (4 * buttonHeight) + (3 * rowSpacing) + markersListHeight;
    int rightBoxHeight = (2 * labelHeight) + (2 * sliderHeight) + (2 * buttonHeight) + (3 * rowSpacing) + 20 + mixerHeight;

    int controlAreaHeight = juce::jmax(leftBoxHeight, rightBoxHeight);
    auto controlArea = area.removeFromTop(controlAreaHeight + 20);
    auto playlistArea = area.reduced(10);

    auto leftBox = controlArea.removeFromLeft(getWidth() / 2).reduced(10);
    auto rightBox = controlArea.reduced(10);

    auto row1 = leftBox.removeFromTop(buttonHeight);
    int buttonWidth = row1.getWidth() / 5;
    loadButton.setBounds(row1.removeFromLeft(buttonWidth).reduced(2));
    restartButton.setBounds(row1.removeFromLeft(buttonWidth).reduced(2));
    loopbutton.setBounds(row1.removeFromLeft(buttonWidth).reduced(2));
    gotostartbutton.setBounds(row1.removeFromLeft(buttonWidth).reduced(2));
    gotoendbutton.setBounds(row1.removeFromLeft(buttonWidth).reduced(2));

    leftBox.removeFromTop(rowSpacing);

    auto row2 = leftBox.removeFromTop(buttonHeight);
    buttonWidth = row2.getWidth() / 3;
    skipBackButton.setBounds(row2.removeFromLeft(buttonWidth).reduced(2));
    playpauseButton.setBounds(row2.removeFromLeft(buttonWidth).reduced(2));
    skipForwardButton.setBounds(row2.removeFromLeft(buttonWidth).reduced(2));

    leftBox.removeFromTop(rowSpacing);

    auto row3 = leftBox.removeFromTop(buttonHeight);
    int fadeButtonWidth = row3.getWidth() / 4;
    fadeInButton.setBounds(row3.removeFromLeft(fadeButtonWidth).reduced(2));
    fadeOutButton.setBounds(row3.removeFromLeft(fadeButtonWidth).reduced(2));
    removeFadesButton.setBounds(row3.removeFromLeft(fadeButtonWidth).reduced(2));
    fadeStatusLabel.setBounds(row3.reduced(2));

    leftBox.removeFromTop(rowSpacing);

    auto row4 = leftBox.removeFromTop(buttonHeight);
    int markerButtonWidth = row4.getWidth() / 2;
    addMarkerButton.setBounds(row4.removeFromLeft(markerButtonWidth).reduced(2));
    clearMarkersButton.setBounds(row4.removeFromLeft(markerButtonWidth).reduced(2));

    markersList.setBounds(leftBox.reduced(2));

    if (mixerEnabled)
    {
        auto mixerArea = rightBox.removeFromTop(mixerHeight);

        auto t1LabelArea = mixerArea.removeFromTop(labelHeight);
        track1MixLabel.setBounds(t1LabelArea);

        auto t1SliderArea = mixerArea.removeFromTop(sliderHeight);
        track1MixSlider.setBounds(t1SliderArea.reduced(2));

        auto t2LabelArea = mixerArea.removeFromTop(labelHeight);
        track2MixLabel.setBounds(t2LabelArea);

        auto t2SliderArea = mixerArea.removeFromTop(sliderHeight);
        track2MixSlider.setBounds(t2SliderArea.reduced(2));

        rightBox.removeFromTop(rowSpacing);
    }

    auto volumeLabelArea = rightBox.removeFromTop(labelHeight);
    volumeLabel.setBounds(volumeLabelArea);

    auto volumeSliderArea = rightBox.removeFromTop(sliderHeight);
    auto muteButtonArea = volumeSliderArea.removeFromRight(80);
    volumeSlider.setBounds(volumeSliderArea.reduced(2));
    mute_button.setBounds(muteButtonArea.reduced(2));

    rightBox.removeFromTop(rowSpacing);

    auto speedLabelArea = rightBox.removeFromTop(labelHeight);
    speedLabel.setBounds(speedLabelArea);

    auto speedSliderArea = rightBox.removeFromTop(sliderHeight);
    speedSlider.setBounds(speedSliderArea.reduced(2));

    auto speedButtonsArea = rightBox.removeFromTop(buttonHeight);
    int speedButtonWidth = speedButtonsArea.getWidth() / 4;
    speedHalfButton.setBounds(speedButtonsArea.removeFromLeft(speedButtonWidth).reduced(2));
    speedNormalButton.setBounds(speedButtonsArea.removeFromLeft(speedButtonWidth).reduced(2));
    speedDoubleButton.setBounds(speedButtonsArea.removeFromLeft(speedButtonWidth).reduced(2));
    speedQuadButton.setBounds(speedButtonsArea.removeFromLeft(speedButtonWidth).reduced(2));

    rightBox.removeFromTop(rowSpacing);

    auto row3Right = rightBox.removeFromTop(buttonHeight);
    int abButtonWidth = row3Right.getWidth() / 5;
    mixerToggleButton.setBounds(row3Right.removeFromLeft(abButtonWidth).reduced(2));
    setPointAButton.setBounds(row3Right.removeFromLeft(abButtonWidth).reduced(2));
    setPointBButton.setBounds(row3Right.removeFromLeft(abButtonWidth).reduced(2));
    clearLoopPointsButton.setBounds(row3Right.removeFromLeft(abButtonWidth).reduced(2));
    toggleSegmentLoopButton.setBounds(row3Right.removeFromLeft(abButtonWidth).reduced(2));

    auto loopLabelsArea = rightBox.removeFromTop(20);
    loopStartLabel.setBounds(loopLabelsArea.removeFromLeft(loopLabelsArea.getWidth() / 2).reduced(2));
    loopEndLabel.setBounds(loopLabelsArea.reduced(2));

    playlist->setBounds(playlistArea);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (!connectedPlayer && button != &mixerToggleButton) return;

    if (button == &mixerToggleButton)
    {
        mixerEnabled = !mixerEnabled;
        mixerToggleButton.setButtonText(mixerEnabled ? "Mixer: On" : "Mixer: Off");
        mixerToggleButton.setColour(juce::TextButton::buttonColourId,
            mixerEnabled ? juce::Colours::purple.withAlpha(0.3f) : juce::Colours::transparentBlack);
        updateMixerControls();
        if (onMixerChanged)
        {
            onMixerChanged(mixerEnabled, track1MixSlider.getValue(), track2MixSlider.getValue());
        }
        updatePlayPauseButton();
        resized();
    }
    else if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                juce::String metadata;
                if (connectedPlayer->loadfile(file, metadata))
                {
                    if (onFileLoaded)
                        onFileLoaded(file);
                }
            });
    }
    else if (button == &restartButton)
    {
        connectedPlayer->setPosition(0.0);
        connectedPlayer->play();
    }
    else if (button == &playpauseButton)
    {
        if (mixerEnabled)
        {
            if (onMixerPlayPauseRequested)
            {
                onMixerPlayPauseRequested();
            }
        }
        else
        {
            if (connectedPlayer->isPlaying())
            {
                connectedPlayer->pause();
            }
            else
            {
                connectedPlayer->play();
            }
        }
        updatePlayPauseButton();
    }
    else if (button == &skipBackButton)
    {
        double currentPos = connectedPlayer->getPosition();
        double newPos = currentPos - 10.0;
        if (newPos < 0.0) newPos = 0.0;
        connectedPlayer->setPosition(newPos);
    }
    else if (button == &skipForwardButton)
    {
        double currentPos = connectedPlayer->getPosition();
        double length = connectedPlayer->getLength();
        double newPos = currentPos + 10.0;
        if (newPos > length) newPos = length;
        connectedPlayer->setPosition(newPos);
    }
    else if (button == &mute_button)
    {
        connectedPlayer->mute();
        if (connectedPlayer->isMuted()) {
            mute_button.setButtonText("Unmute");
            volumeSlider.setValue(0.0);
        }
        else {
            mute_button.setButtonText("Mute");
            volumeSlider.setValue(connectedPlayer->getPreviousVolume());
        }
    }
    else if (button == &loopbutton)
    {
        bool currentlylooping = connectedPlayer->islooping();
        connectedPlayer->setlooping(!currentlylooping);
        loopbutton.setButtonText(currentlylooping ? "Loop: Off" : "Loop: On");
    }
    else if (button == &gotostartbutton)
    {
        connectedPlayer->setPosition(0.0);
    }
    else if (button == &gotoendbutton)
    {
        double length = connectedPlayer->getLength();
        if (length > 0.0)
        {
            connectedPlayer->setPosition(length);
        }
    }
    else if (button == &speedHalfButton)
    {
        speedSlider.setValue(0.5);
    }
    else if (button == &speedNormalButton)
    {
        speedSlider.setValue(1.0);
    }
    else if (button == &speedDoubleButton)
    {
        speedSlider.setValue(2.0);
    }
    else if (button == &speedQuadButton)
    {
        speedSlider.setValue(4.0);
    }
    else if (button == &setPointAButton)
    {
        double currentPos = connectedPlayer->getPosition();
        connectedPlayer->setLoopPoints(currentPos, connectedPlayer->getLoopEnd());
        updateLoopLabels();
    }
    else if (button == &setPointBButton)
    {
        double currentPos = connectedPlayer->getPosition();
        connectedPlayer->setLoopPoints(connectedPlayer->getLoopStart(), currentPos);
        updateLoopLabels();
    }
    else if (button == &clearLoopPointsButton)
    {
        connectedPlayer->clearLoopPoints();
        updateLoopLabels();
        toggleSegmentLoopButton.setButtonText("A-B Loop: Off");
    }
    else if (button == &toggleSegmentLoopButton)
    {
        if (connectedPlayer->isSegmentLooping()) {
            connectedPlayer->clearLoopPoints();
            toggleSegmentLoopButton.setButtonText("A-B Loop: Off");
        }
        else {
            if (connectedPlayer->getLoopEnd() > connectedPlayer->getLoopStart()) {
                toggleSegmentLoopButton.setButtonText("A-B Loop: On");
            }
        }
        updateLoopLabels();
    }
    else if (button == &fadeInButton)
    {
        connectedPlayer->applyFadeIn();
        updateFadeStatus();
    }
    else if (button == &fadeOutButton)
    {
        connectedPlayer->applyFadeOut();
        updateFadeStatus();
    }
    else if (button == &removeFadesButton)
    {
        connectedPlayer->removeFades();
        updateFadeStatus();
    }
    else if (button == &addMarkerButton)
    {
        if (connectedPlayer) {
            double currentTime = connectedPlayer->getPosition();
            connectedPlayer->addMarker(currentTime);
            updateMarkersList();
        }
    }
    else if (button == &clearMarkersButton)
    {
        if (connectedPlayer) {
            connectedPlayer->removeAllMarkers();
            updateMarkersList();
        }
    }
}

int PlayerGUI::getNumRows()
{
    return connectedPlayer ? connectedPlayer->getNumMarkers() : 0;
}

void PlayerGUI::paintListBoxItem(int rowNumber, juce::Graphics& g,
    int width, int height, bool rowIsSelected)
{
    if (rowNumber >= getNumRows()) return;

    auto markers = connectedPlayer->getMarkers();
    if (rowNumber < markers.size()) {
        auto& marker = markers[rowNumber];

        auto formatTime = [](double seconds) {
            int mins = static_cast<int>(seconds) / 60;
            int secs = static_cast<int>(seconds) % 60;
            int ms = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);
            return juce::String::formatted("%02d:%02d.%03d", mins, secs, ms);
            };

        if (rowIsSelected) {
            g.setColour(juce::Colours::white.withAlpha(0.3f));
            g.fillRect(0, 0, width, height);
        }

        g.setColour(juce::Colours::white);
        g.setFont(14.0f);

        juce::String text = formatTime(marker.first) + " - " + marker.second;
        g.drawText(text, 5, 0, width - 5, height, juce::Justification::centredLeft);
    }
}

void PlayerGUI::listBoxItemDoubleClicked(int row, const juce::MouseEvent&)
{
    if (connectedPlayer && row >= 0 && row < connectedPlayer->getNumMarkers()) {
        connectedPlayer->jumpToMarker(row);
    }
}

void PlayerGUI::deleteKeyPressed(int lastRowSelected)
{
    if (connectedPlayer && lastRowSelected >= 0 && lastRowSelected < connectedPlayer->getNumMarkers()) {
        connectedPlayer->removeMarker(lastRowSelected);
        updateMarkersList();
    }
}

void PlayerGUI::updateMarkersList()
{
    markersList.updateContent();
    markersList.repaint();
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &track1MixSlider || slider == &track2MixSlider)
    {
        float vol1 = static_cast<float>(track1MixSlider.getValue());
        float vol2 = static_cast<float>(track2MixSlider.getValue());

        track1MixLabel.setText("Track 1: " + juce::String(static_cast<int>(vol1 * 100)) + "%", juce::dontSendNotification);
        track2MixLabel.setText("Track 2: " + juce::String(static_cast<int>(vol2 * 100)) + "%", juce::dontSendNotification);

        if (onMixerChanged)
        {
            onMixerChanged(mixerEnabled, vol1, vol2);
        }
        return;
    }

    if (!connectedPlayer) return;

    if (slider == &volumeSlider)
    {
        float volumeValue = static_cast<float>(volumeSlider.getValue());
        connectedPlayer->setGain(volumeValue);
        volumeLabel.setText("Volume: " + juce::String(static_cast<int>(volumeValue * 100)) + "%", juce::dontSendNotification);
    }
    else if (slider == &speedSlider)
    {
        float speedValue = static_cast<float>(speedSlider.getValue());
        connectedPlayer->setSpeed(speedValue);
        speedLabel.setText("Speed: " + juce::String(speedValue, 1) + "x", juce::dontSendNotification);
    }
}

void PlayerGUI::updatePlayPauseButton()
{
    if (mixerEnabled)
    {
        playpauseButton.setButtonText("|| Pause / > Play");
        playpauseButton.repaint();
        return;
    }

    if (!connectedPlayer)
    {
        playpauseButton.setButtonText("> Play");
        return;
    }

    playpauseButton.setButtonText(connectedPlayer->isPlaying() ? "|| Pause" : "> Play");
    playpauseButton.repaint();
}

void PlayerGUI::connectToPlayer(PlayerAudio* player)
{
    connectedPlayer = player;
    refreshUI();
}

void PlayerGUI::refreshUI()
{
    if (!connectedPlayer) return;

    updatePlayPauseButton();
    updateLoopLabels();
    updateFadeStatus();
    updateMarkersList();

    volumeSlider.setValue(connectedPlayer->getGain(), juce::dontSendNotification);
    volumeLabel.setText("Volume: " + juce::String(static_cast<int>(connectedPlayer->getGain() * 100)) + "%",
                       juce::dontSendNotification);

    speedSlider.setValue(connectedPlayer->getSpeed(), juce::dontSendNotification);
    speedLabel.setText("Speed: " + juce::String(connectedPlayer->getSpeed(), 1) + "x",
                      juce::dontSendNotification);

    mute_button.setButtonText(connectedPlayer->isMuted() ? "Unmute" : "Mute");

    loopbutton.setButtonText(connectedPlayer->islooping() ? "Loop: On" : "Loop: Off");

    toggleSegmentLoopButton.setButtonText(connectedPlayer->isSegmentLooping() ? "A-B Loop: On" : "A-B Loop: Off");
}

void PlayerGUI::updateLoopLabels()
{
    if (!connectedPlayer) return;

    auto formatTime = [](double seconds) {
        if (seconds < 0.0) seconds = 0.0;
        int mins = static_cast<int>(seconds) / 60;
        int secs = static_cast<int>(seconds) % 60;
        return juce::String::formatted("%d:%02d", mins, secs);
        };

    loopStartLabel.setText("A: " + formatTime(connectedPlayer->getLoopStart()), juce::dontSendNotification);
    loopEndLabel.setText("B: " + formatTime(connectedPlayer->getLoopEnd()), juce::dontSendNotification);
}

void PlayerGUI::updateFadeStatus()
{
    if (!connectedPlayer) return;

    bool fadeIn = connectedPlayer->hasFadeInApplied();
    bool fadeOut = connectedPlayer->hasFadeOutApplied();

    if (fadeIn && fadeOut) {
        fadeStatusLabel.setText("Fades: Both Applied", juce::dontSendNotification);
    }
    else if (fadeIn) {
        fadeStatusLabel.setText("Fades: In Only", juce::dontSendNotification);
    }
    else if (fadeOut) {
        fadeStatusLabel.setText("Fades: Out Only", juce::dontSendNotification);
    }
    else {
        fadeStatusLabel.setText("Fades: None", juce::dontSendNotification);
    }
}

void PlayerGUI::updateMixerControls()
{
    track1MixSlider.setVisible(mixerEnabled);
    track2MixSlider.setVisible(mixerEnabled);
    track1MixLabel.setVisible(mixerEnabled);
    track2MixLabel.setVisible(mixerEnabled);
}