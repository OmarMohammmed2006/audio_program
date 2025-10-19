#pragma once
#include <JuceHeader.h>

class PlayerAudio {
public:
	PlayerAudio();
	~PlayerAudio();

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
	void releaseResources();

	bool loadfile(const juce::File& file);
	void start();
	void pause();
	void play();
	void mute();
	void setSpeed();

	void setGain(float gain);
	float getGain() { return currentGain; }
	bool getMuteState() { return isMuted; }
	float getPreviousVolume() const { return previousVolume; }
	void setPosition(double pos);
	double getPosition() const;
	double getLength() const;
private:
	juce::AudioFormatManager formatManager;
	std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
	juce::AudioTransportSource transportSource;

	float previousVolume = 0.5f;
	bool isMuted = false;
	float currentSpeed = 1.0f;
	float currentGain = 0.5f;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};