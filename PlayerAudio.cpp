#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
	formatManager.registerBasicFormats();
}
PlayerAudio::~PlayerAudio()
{
	transportSource.setSource(nullptr);
}
void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	transportSource.getNextAudioBlock(bufferToFill);
}
void PlayerAudio::releaseResources()
{
	transportSource.releaseResources();
}
bool PlayerAudio::loadfile(const juce::File& file)
{
    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {
            // ?? Disconnect old source first
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();

            // Create new reader source
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            // Attach safely
            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);
            transportSource.start();
			return true;
        }
		return false;
    }
	return false;
}
void PlayerAudio::start()
{
    transportSource.start();
}

void PlayerAudio::pause()
{
    transportSource.stop();
	transportSource.setPosition(transportSource.getCurrentPosition());
}

void PlayerAudio::play()
{
    transportSource.setPosition(transportSource.getCurrentPosition());
    transportSource.start();
}

void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);
}

void PlayerAudio::setPosition(double pos)
{
    transportSource.setPosition(pos);
}

double PlayerAudio::getPosition() const
{
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getLength() const
{
    return transportSource.getLengthInSeconds();
}