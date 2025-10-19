#include "audio_program/PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
	for (auto* btn : { &loadButton, &restartButton , &pauseButton, &playButton, &gotostartbutton, &gotoendbutton, &mute_button})
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

    pauseButton.setBounds(x, y, buttonWidth, buttonHeight);
    x += buttonWidth + spacing;

    playButton.setBounds(x, y, buttonWidth, buttonHeight);
    x += buttonWidth + spacing;

    gotostartbutton.setBounds(x, y, buttonWidth + 10, buttonHeight);
    x += buttonWidth+10 + spacing;

    gotoendbutton.setBounds(x, y, buttonWidth, buttonHeight);
    x += buttonWidth + spacing;

    mute_button.setBounds(x, y, buttonWidth, buttonHeight);

    volumeSlider.setBounds(fixed, 90, getWidth() - 40, 30);
    speedSlider.setBounds(fixed, 130, getWidth() - 40, 30);

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
                playerAudio.loadfile(file);
            });
    }

    if (button == &restartButton)
    {
        playerAudio.setPosition(0.0);
        playerAudio.start();
    }

    if (button == &pauseButton)
    {
        playerAudio.pause();
    }

    if (button == &playButton)
    {
        playerAudio.play();
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
            playerAudio.pause();
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

}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        playerAudio.setGain(static_cast<float>(volumeSlider.getValue()));
    }
}