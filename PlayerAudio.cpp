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
    if (hasFadeIn || hasFadeOut)
    {
        double currentTime = getPosition();
        double fadeGain = calculateFadeGain(currentTime);
        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            float* channelData = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);

            for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                channelData[sample] *= static_cast<float>(fadeGain);
            }
        }
    }
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
void PlayerAudio::applyFadeIn()
{
    double totalLength = getLength();
    fadeInDuration = 0.1 * totalLength;
    hasFadeIn = true;
}
void PlayerAudio::applyFadeOut()
{
    double totalLength = getLength();
    fadeOutDuration = 0.1 * totalLength;
    hasFadeOut = true;
}
void PlayerAudio::removeFades()
{
    hasFadeIn = false;
    hasFadeOut = false;
}
double PlayerAudio::calculateFadeGain(double currentTime)
{
    double totalLength = getLength();
    double gain = 1.0f;
    if (hasFadeIn && currentTime < fadeInDuration && fadeInDuration > 0.0)
    {
        gain *= (currentTime / fadeInDuration);
    }
    if (hasFadeOut && currentTime > (totalLength - fadeOutDuration) && fadeOutDuration > 0.0)
    {
        double fadeOutStart = totalLength - fadeOutDuration;
        double fadeProgress = (currentTime - fadeOutStart) / fadeOutDuration;
        gain *= (1.0 - fadeProgress);
    }

    return juce::jlimit(0.0f, 1.0f, static_cast<float>(gain));
}

void PlayerAudio::addMarker(double time, const juce::String& name)
{
    time = juce::jlimit(0.0, getLength(), time);

    juce::String markerName = name;
    if (markerName.isEmpty()) {
        markerName = "Marker " + juce::String(markers.size() + 1);
    }

    markers.emplace_back(time, markerName);

    std::sort(markers.begin(), markers.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
}

void PlayerAudio::removeMarker(int index)
{
    if (index >= 0 && index < markers.size()) {
        markers.erase(markers.begin() + index);
    }
}

void PlayerAudio::removeAllMarkers()
{
    markers.clear();
}

void PlayerAudio::jumpToMarker(int index)
{
    if (index >= 0 && index < markers.size()) {
        setPosition(markers[index].first);
    }
}