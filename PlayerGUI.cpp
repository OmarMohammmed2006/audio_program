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

    for (auto* btn : { &loadButton, &restartButton, &playButton,
                      &gotostartbutton, &gotoendbutton, &mute_button, &loopbutton, &skipBackButton, &skipForwardButton})
    {
        addAndMakeVisible(btn);
        btn->addListener(this);
        setupModernButton(*btn);
    }


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
    for (auto* btn : { &loadButton, &restartButton, &playButton,
                     &gotostartbutton, &gotoendbutton, &mute_button, &loopbutton, &skipBackButton, &skipForwardButton})
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
    const int y = 20;  // Start from top since no waveform
    int x = 10;

    loadButton.setBounds(x, y, buttonWidth+20, buttonHeight);
    x += buttonWidth + 20 + spacing;

    restartButton.setBounds(x, y, buttonWidth, buttonHeight);
    x += buttonWidth + spacing;

    playButton.setBounds(x, y, buttonWidth, buttonHeight);
    x += buttonWidth + spacing;

    skipBackButton.setBounds(x, y, buttonWidth - 20, buttonHeight);
    x += buttonWidth - 20 + spacing;

    skipForwardButton.setBounds(x, y, buttonWidth - 20, buttonHeight);
    x += buttonWidth - 20 + spacing;

    gotostartbutton.setBounds(x, y, buttonWidth + 10, buttonHeight);
    x += buttonWidth+10 + spacing;

    gotoendbutton.setBounds(x, y, buttonWidth, buttonHeight);
    x += buttonWidth + spacing;

    mute_button.setBounds(x, y, buttonWidth, buttonHeight);
    x += buttonWidth + spacing;

    loopbutton.setBounds(x, y, buttonWidth, buttonHeight);

    volumeLabel.setBounds(10, 80, getWidth() - 20, 20);
    volumeSlider.setBounds(10, 100, getWidth() - 20, 30);

    speedLabel.setBounds(10, 140, getWidth() - 20, 20);
    speedSlider.setBounds(10, 160, getWidth() - 20, 30);
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
                if (connectedPlayer->loadfile(file))
                {
                    // Notify MainComponent to update the waveform
                    if (onFileLoaded)
                        onFileLoaded(file);
                }
            });
    }

    if (button == &restartButton)
    {
        connectedPlayer->setPosition(0.0);
        connectedPlayer->start();
    }

    if (button == &playButton)
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

    if (button == &gotostartbutton)
    {
        connectedPlayer->setPosition(0.0);
    }

    if (button == &gotoendbutton)
    {
        double length = connectedPlayer->getLength();
        if (length > 0.0)
        {
            connectedPlayer->setPosition(length);
            connectedPlayer->pause();
        }
    }

    if (button == &mute_button)
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

    if (button == &loopbutton)
    {
        bool currentlylooping = connectedPlayer->islooping();
        connectedPlayer->setlooping(!currentlylooping);
        if (currentlylooping)
        {
            loopbutton.setButtonText("Loop: Off");
        }
        else
        {
            loopbutton.setButtonText("Loop: On");
        }
    }
    if (button == &skipBackButton)
    {
        double currentPos = connectedPlayer->getPosition();
        double newPos = currentPos - 10.0;
        if (newPos < 0.0) newPos = 0.0;
        connectedPlayer->setPosition(newPos);
    }

    if (button == &skipForwardButton)
    {
        double currentPos = connectedPlayer->getPosition();
        double length = connectedPlayer->getLength();
        double newPos = currentPos + 10.0;
        if (newPos > length) newPos = length;
        connectedPlayer->setPosition(newPos);
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (!connectedPlayer) return;

    if (slider == &volumeSlider)
    {
        float volumeValue = static_cast<float>(volumeSlider.getValue());
        connectedPlayer->setGain(volumeValue);

        int volumePercent = static_cast<int>(volumeValue * 100);
        volumeLabel.setText("Volume: " + juce::String(volumePercent) + "%", juce::dontSendNotification);
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
        playButton.setButtonText("> Play");
        return;
    }

    if (connectedPlayer->isPlaying())
    {
        playButton.setButtonText("|| Pause");
    }
    else
    {
        playButton.setButtonText("> Play");
    }

    playButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    playButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);

    playButton.repaint();
}

void PlayerGUI::connectToPlayer(PlayerAudio* player)
{
    connectedPlayer = player;
    updatePlayPauseButton();
}
