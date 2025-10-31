#include "PlayerAudio.h"

PlayerAudio::PlayerAudio() : resamplerSource(&transportSource, false)
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
    resamplerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    checkLoopBoundaries();
    resamplerSource.getNextAudioBlock(bufferToFill);
    checkLoopBoundaries();
}

void PlayerAudio::releaseResources()
{
    resamplerSource.releaseResources();
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
            setSpeed(1.0f);

            juce::StringArray metadataLines;
            auto& metadataValues = reader->metadataValues;

            metadataLines.add("File: " + file.getFileName());
            metadataLines.add("Duration: " + juce::String(transportSource.getLengthInSeconds(), 1) + " seconds");

            static const juce::String emptyString;
            const juce::String title = metadataValues.getValue("title", emptyString);
            const juce::String artist = metadataValues.getValue("artist", emptyString);
            const juce::String album = metadataValues.getValue("album", emptyString);

            if (!title.isEmpty()) metadataLines.add("Title: " + title);
            if (!artist.isEmpty()) metadataLines.add("Artist: " + artist);
            if (!album.isEmpty()) metadataLines.add("Album: " + album);

            metadata = metadataLines.joinIntoString("\n");
            return true;
        }
        metadata = "Error: Could not read file";
        return false;
    }
    metadata = "Error: File does not exist";
    return false;
}

void PlayerAudio::play()
{
    transportSource.start();
}

void PlayerAudio::pause()
{
    transportSource.stop();
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

void PlayerAudio::setSpeed(float newSpeed) {
    newSpeed = juce::jlimit(0.25f, 4.0f, newSpeed);

    if (currentSpeed != newSpeed)
    {
        currentSpeed = newSpeed;
        double resamplingRatio = currentSpeed;
        resamplerSource.setResamplingRatio(resamplingRatio);
    }
}
void PlayerAudio::setLoopPoints(double startTime, double endTime)
{
    loopStartTime = juce::jmax(0.0, startTime);
    loopEndTime = juce::jmin(getLength(), endTime);
    
    if (loopEndTime <= loopStartTime)
    {
        loopEndTime = loopStartTime + 1.0;
        if (loopEndTime > getLength())
        {
            loopEndTime = getLength();
            if (loopStartTime >= loopEndTime)
            {
                isSegmentLoopEnabled = false;
                return;
            }
        }
    }

    isSegmentLoopEnabled = true;
}

void PlayerAudio::clearLoopPoints()
{
    isSegmentLoopEnabled = false;
    loopStartTime = 0.0;
    loopEndTime = getLength();
}
void PlayerAudio::checkLoopBoundaries()
{
    if (!isSegmentLoopEnabled || !readerSource) return;

    double currentPos = getPosition();

    if (currentPos >= loopEndTime && currentPos > previousPosition)
    {
        setPosition(loopStartTime);
    }

    previousPosition = currentPos;
}
