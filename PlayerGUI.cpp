#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
	// Add buttons
	for (auto* btn : { &loadButton, &restartButton , &pauseButton, &playButton })
	{
		addAndMakeVisible(btn);
		btn->addListener(this);
	}
	// Volume slider
	volumeSlider.setRange(0.0, 1.0, 0.01);
	volumeSlider.setValue(0.5);
	volumeSlider.addListener(this);
	addAndMakeVisible(volumeSlider);
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
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    restartButton.setBounds(140, y, 80, 40);
    pauseButton.setBounds(240, y, 80, 40);
    playButton.setBounds(340, y, 80, 40);
    volumeSlider.setBounds(20, y+60, getWidth() - 40, 30);
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

}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        // Set the gain of the playerAudio based on the slider value
        playerAudio.setGain(static_cast<float>(volumeSlider.getValue()));
    }
}