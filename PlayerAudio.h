#pragma once
#include <JuceHeader.h>

class PlayerAudio {
public:
	PlayerAudio();
	~PlayerAudio();

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
	void releaseResources();

	bool loadfile(const juce::File& file, juce::String& metadata);
	void play();
	void pause();
	bool isPlaying() const { return transportSource.isPlaying(); }

	void mute();
	bool getMuteState() { return isMuted; }
	float getPreviousVolume() const { return previousVolume; }

	void setlooping(bool shouldloop);
	bool islooping() const { return isloopingenabled; }

	void setGain(float gain);
	float getGain() { return currentGain; }

	void setPosition(double pos);
	double getPosition() const;
	double getLength() const;

	void setSpeed(float newSpeed);
	float getSpeed() const { return currentSpeed; }

private:
	juce::AudioFormatManager formatManager;
	std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
	juce::AudioTransportSource transportSource;
	juce::ResamplingAudioSource resamplerSource;

	float previousVolume = 0.5f;
	bool isMuted = false;
	float currentSpeed = 1.0f;
	float currentGain = 0.5f;
	bool isloopingenabled = false;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};