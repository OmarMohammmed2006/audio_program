#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
	for (auto* btn : { &loadButton, &restartButton , &playpauseButton, &gotostartbutton, &gotoendbutton, &mute_button, &loopbutton})
	{
		addAndMakeVisible(btn);
		btn->addListener(this);
	}
	volumeSlider.setRange(0.0, 1.0, 0.01);
	volumeSlider.setValue(0.5);
	volumeSlider.addListener(this);
	addAndMakeVisible(volumeSlider);
    speedSlider.setRange(0.25, 4.0, 0.1);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);

    metadataLabel.setText("No file loaded", juce::dontSendNotification);
    metadataLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(metadataLabel);
}

PlayerGUI::~PlayerGUI() {}

void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	playerAudio.getNextAudioBlock(bufferToFill);
}
void PlayerGUI::releaseResources()
{
	playerAudio.releaseResources();
}

void PlayerGUI::resized()
{
    const int fixed = 10;
    const int buttonWidth = 80;
    const int buttonHeight = 40;
    const int spacing = 10;
    const int y = fixed;
    int x = fixed;

    loadButton.setBounds(x, y, buttonWidth+20, buttonHeight);
    x += buttonWidth+20 + spacing;

    restartButton.setBounds(x, y, buttonWidth, buttonHeight);
    x += buttonWidth + spacing;

    playpauseButton.setBounds(x, y, buttonWidth, buttonHeight);
    x += buttonWidth + spacing;

    gotostartbutton.setBounds(x, y, buttonWidth + 10, buttonHeight);
    x += buttonWidth+10 + spacing;

    gotoendbutton.setBounds(x, y, buttonWidth, buttonHeight);
    x += buttonWidth + spacing;

    mute_button.setBounds(x, y, buttonWidth, buttonHeight);
    x += buttonWidth + spacing;

    loopbutton.setBounds(x, y, buttonWidth, buttonHeight);

    volumeSlider.setBounds(fixed, 90, getWidth() - 40, 30);
    speedSlider.setBounds(fixed, 130, getWidth() - 40, 30);
    metadataLabel.setBounds(fixed, 170, getWidth() - 40, 100);

}

void PlayerGUI::buttonClicked(juce::Button* button)
{
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
                if (playerAudio.loadfile(file, metadata))
                {
                    updateMetadataDisplay(metadata);
                }
            });
    }

    if (button == &restartButton)
    {
        playerAudio.setPosition(0.0);
        playerAudio.start();
    }

    if (button == &playpauseButton)
    {
        playerAudio.pause();
        if (playerAudio.getPauseState()) {
            playpauseButton.setButtonText("Play");
        }
        else {
            playpauseButton.setButtonText("Pause");
		}
    }

    if (button == &gotostartbutton)
    {
        playerAudio.setPosition(0.0);
	}

    if (button == &gotoendbutton)
    {
        double length = playerAudio.getLength();
        if (length > 0.0)
        {
            playerAudio.setPosition(length);
        }
    }
    if (button == &mute_button)
    {
        playerAudio.mute();
        if (playerAudio.getMuteState()) {
            mute_button.setButtonText("Unmute");
            volumeSlider.setValue(0.0);
        } else {
            mute_button.setButtonText("Mute");
            volumeSlider.setValue(playerAudio.getPreviousVolume());
        }
    }
    if (button == &loopbutton)
    {
        bool currentlylooping = playerAudio.islooping();
        playerAudio.setlooping(!currentlylooping);
        if (currentlylooping)
        {
            loopbutton.setButtonText("Loop: Off");
        }
        else
        {
            loopbutton.setButtonText("Loop: On");
        }
    }

}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        playerAudio.setGain(static_cast<float>(volumeSlider.getValue()));
    }
}

void PlayerGUI::updateMetadataDisplay(const juce::String& metadata)
{
    metadataLabel.setText(metadata, juce::dontSendNotification);
}