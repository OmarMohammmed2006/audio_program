#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
	public juce::Button::Listener,
	public juce::Slider::Listener
{
public:
	PlayerGUI();
	~PlayerGUI() override;

	void resized() override;

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
	void releaseResources();

private:
	PlayerAudio playerAudio;

	juce::TextButton loadButton{ "Load Files" };
	juce::TextButton restartButton{ "Restart" };
	juce::TextButton pauseButton{ "Pause" };
	juce::TextButton playButton{ "Play" };
	juce::TextButton gotostartbutton{ "Go To Start" };
	juce::TextButton gotoendbutton{ "Go To End" };
	juce::TextButton mute_button{ "Mute" };
	juce::TextButton loopbutton{ "Loop: off" };
	juce::Slider volumeSlider;
	juce::Slider speedSlider;


	std::unique_ptr<juce::FileChooser> fileChooser;
	void buttonClicked(juce::Button* button) override;
	void sliderValueChanged(juce::Slider* slider) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};	