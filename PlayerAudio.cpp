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
bool PlayerAudio::loadfile(const juce::File& file, juce::String& metadata)
{
    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);

            juce::StringArray metadataLines;
            auto metadataValues = reader->metadataValues;
            static const juce::String emptyString;

            if (metadataValues.size() == 0)
            {
                metadata = "Filename: " + file.getFileName();
            }
            else
            {
                // Use juce::String instead of const juce::String*
                const juce::String title = metadataValues.getValue("title", emptyString);
                if (!title.isEmpty())
                    metadataLines.add("Title: " + title);

                const juce::String artist = metadataValues.getValue("artist", emptyString);
                if (!artist.isEmpty())
                    metadataLines.add("Artist: " + artist);

                const juce::String album = metadataValues.getValue("album", emptyString);
                if (!album.isEmpty())
                    metadataLines.add("Album: " + album);

                metadataLines.add("Duration: " + juce::String(transportSource.getLengthInSeconds()) + " seconds");
                metadata = metadataLines.joinIntoString("\n");
            }

            transportSource.start();
            return true;
        }
        metadata = "Error: Could not read file";
        return false;
    }
    metadata = "Error: File does not exist";
    return false;
}
void PlayerAudio::start()
{
    transportSource.start();
}

void PlayerAudio::pause()
{
	isPlaying = !isPlaying;
    if (isPlaying) {
        transportSource.stop();
        transportSource.setPosition(transportSource.getCurrentPosition());
    }
    else
    {
        transportSource.setPosition(transportSource.getCurrentPosition());
        transportSource.start();
    }
    
}

void PlayerAudio::mute() {
    isMuted = !isMuted;
    if (isMuted) {
        previousVolume = currentGain;
        setGain(0.0f);
    } else {
        setGain(previousVolume);
    }
}


void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);
    currentGain = gain;

    if (isMuted && gain > 0.0f) {
        isMuted = false;
    }
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

void PlayerAudio::setlooping(bool shouldloop)
{
    isloopingenabled = shouldloop;
    if (readerSource) readerSource->setLooping(shouldloop);
}
