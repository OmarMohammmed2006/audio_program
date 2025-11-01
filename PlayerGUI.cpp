#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    auto setupModernButton = [this](juce::TextButton& button) {
        button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
        button.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
        button.setColour(juce::TextButton::buttonOnColourId, juce::Colours::white.withAlpha(0.3f));
        button.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        button.setColour(juce::TextButton::textColourOffId, juce::Colours::white.withAlpha(0.9f));
        button.setLookAndFeel(&customLookAndFeel);
    };

    for (auto* btn : { &loadButton, &restartButton, &playpauseButton,
        &skipBackButton, &skipForwardButton, &mute_button, &loopbutton,&gotostartbutton,&gotoendbutton,
        &speedHalfButton, &speedNormalButton, &speedDoubleButton, &speedQuadButton, &setPointAButton, &setPointBButton,
        &clearLoopPointsButton, &toggleSegmentLoopButton, &fadeInButton, &fadeOutButton, &removeFadesButton })
    {
        addAndMakeVisible(btn);
        btn->addListener(this);
        setupModernButton(*btn);
    }

    fadeStatusLabel.setText("Fades: None", juce::dontSendNotification);
    fadeStatusLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.9f));
    fadeStatusLabel.setJustificationType(juce::Justification::centredLeft);
    fadeStatusLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    addAndMakeVisible(fadeStatusLabel);
    loopStartLabel.setText("A: --:--", juce::dontSendNotification);
    loopEndLabel.setText("B: --:--", juce::dontSendNotification);
    loopStartLabel.setColour(juce::Label::textColourId, juce::Colours::yellow.withAlpha(0.9f));
    loopEndLabel.setColour(juce::Label::textColourId, juce::Colours::yellow.withAlpha(0.9f));
    loopStartLabel.setJustificationType(juce::Justification::centredLeft);
    loopEndLabel.setJustificationType(juce::Justification::centredRight);
    loopStartLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    loopEndLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    addAndMakeVisible(loopStartLabel);
    addAndMakeVisible(loopEndLabel);
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(volumeSlider);

    volumeLabel.setText("Volume: 50%", juce::dontSendNotification);
    volumeLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.9f));
    volumeLabel.setJustificationType(juce::Justification::centredLeft);
    volumeLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    addAndMakeVisible(volumeLabel);

    speedSlider.setRange(0.25, 4.0, 0.1);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    speedSlider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(speedSlider);

    speedLabel.setText("Speed: 1.0x", juce::dontSendNotification);
    speedLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.9f));
    speedLabel.setJustificationType(juce::Justification::centredLeft);
    speedLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    addAndMakeVisible(speedLabel);
}

PlayerGUI::~PlayerGUI()
{
    for (auto* btn : { &loadButton, &restartButton, &playpauseButton,
        &skipBackButton, &skipForwardButton, &mute_button, &loopbutton,&gotostartbutton,&gotoendbutton,
        &speedHalfButton, &speedNormalButton, &speedDoubleButton, &speedQuadButton})

    {
        btn->setLookAndFeel(nullptr);
    }
    volumeSlider.setLookAndFeel(nullptr);
    speedSlider.setLookAndFeel(nullptr);
}

void PlayerGUI::resized()
{
    const int buttonWidth = 80;
    const int buttonHeight = 40;
    const int spacing = 10;
    const int y = 20;
    int x = 10;

    loadButton.setBounds(x, y, buttonWidth + 20, buttonHeight);
    x += buttonWidth + 20 + spacing;

    restartButton.setBounds(x, y, buttonWidth, buttonHeight);
    x += buttonWidth + spacing;

    playpauseButton.setBounds(x, y, buttonWidth, buttonHeight);
    x += buttonWidth + spacing;

    skipBackButton.setBounds(x, y, buttonWidth - 20, buttonHeight);
    x += buttonWidth - 20 + spacing;

    skipForwardButton.setBounds(x, y, buttonWidth - 20, buttonHeight);
    x += buttonWidth - 20 + spacing;

    mute_button.setBounds(x, y, buttonWidth, buttonHeight);
    x += buttonWidth + spacing;

    loopbutton.setBounds(x, y, buttonWidth, buttonHeight);
    x += buttonWidth + spacing;

    gotostartbutton.setBounds(x, y, buttonWidth + 10, buttonHeight);
    x += buttonWidth+10 + spacing;

    gotoendbutton.setBounds(x, y, buttonWidth, buttonHeight);
    x += buttonWidth + spacing;

    volumeLabel.setBounds(10, 80, getWidth() - 20, 20);
    volumeSlider.setBounds(10, 100, getWidth() - 20, 30);

    speedLabel.setBounds(10, 140, getWidth() - 20, 20);
    speedSlider.setBounds(10, 160, getWidth() - 20, 30);

    const int speedButtonWidth = 60;
    const int speedButtonHeight = 30;
    const int speedY = 200;
    int speedX = 10;
    const int abButtonWidth = 80;
    const int abButtonHeight = 30;
    const int abY = 325;
    int abX = 10;
    const int fadeButtonWidth = 80;
    const int fadeButtonHeight = 30;
    const int fadeY = 380;
    int fadeX = 10;
    fadeInButton.setBounds(fadeX, fadeY, fadeButtonWidth, fadeButtonHeight);
    fadeX += fadeButtonWidth + 5;
    fadeOutButton.setBounds(fadeX, fadeY, fadeButtonWidth, fadeButtonHeight);
    fadeX += fadeButtonWidth + 5;
    removeFadesButton.setBounds(fadeX, fadeY, fadeButtonWidth + 10, fadeButtonHeight);
    fadeStatusLabel.setBounds(10, fadeY + 35, getWidth() - 20, 20);

    setPointAButton.setBounds(abX, abY, abButtonWidth, abButtonHeight);
    abX += abButtonWidth + 5;

    setPointBButton.setBounds(abX, abY, abButtonWidth, abButtonHeight);
    abX += abButtonWidth + 5;

    clearLoopPointsButton.setBounds(abX, abY, abButtonWidth, abButtonHeight);
    abX += abButtonWidth + 5;

    toggleSegmentLoopButton.setBounds(abX, abY, abButtonWidth + 20, abButtonHeight);

    loopStartLabel.setBounds(10, abY + 35, 80, 20);
    loopEndLabel.setBounds(getWidth() - 90, abY + 35, 80, 20);

    speedHalfButton.setBounds(speedX, speedY, speedButtonWidth, speedButtonHeight);
    speedX += speedButtonWidth + spacing;

    speedNormalButton.setBounds(speedX, speedY, speedButtonWidth, speedButtonHeight);
    speedX += speedButtonWidth + spacing;

    speedDoubleButton.setBounds(speedX, speedY, speedButtonWidth, speedButtonHeight);
    speedX += speedButtonWidth + spacing;

    speedQuadButton.setBounds(speedX, speedY, speedButtonWidth, speedButtonHeight);

}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (!connectedPlayer) return;

    if (button == &loadButton)
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
                    // Metadata is handled by MainComponent
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
        if (connectedPlayer->isPlaying())
        {
            connectedPlayer->pause();
        }
        else
        {
            connectedPlayer->play();
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
        if (connectedPlayer->getMuteState()) {
            mute_button.setButtonText("Unmute");
            volumeSlider.setValue(0.0);
        } else {
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
        // In practice, this will be set by the waveform click in MainComponent
        // For now, just set at current position
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
        } else {
            // Enable segment looping if points are set
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
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
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
    updatePlayPauseButton();
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