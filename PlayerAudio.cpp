#include "PlayerAudio.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>


PlayerAudio::PlayerAudio()
    : resamplerSource(&transportSource, false)
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

    if (fadeInEnabled || fadeOutEnabled)
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
    if (!file.existsAsFile())
    {
        metadata = "Error: File does not exist";
        return false;
    }

    auto* reader = formatManager.createReaderFor(file);
    if (!reader)
    {
        metadata = "Error: Could not read file";
        return false;
    }

    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();

    readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
    transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
    setSpeed(1.0f);

    // Read metadata with TagLib
    TagLib::FileRef audioFile(file.getFullPathName().toRawUTF8());

    juce::String title = "Unknown";
    juce::String artist = "Unknown";
    juce::String album = "Unknown";

    if (!audioFile.isNull() && audioFile.tag())
    {
        TagLib::Tag *tag = audioFile.tag();
        title = juce::String(tag->title().toCString(true));
        artist = juce::String(tag->artist().toCString(true));
        album = juce::String(tag->album().toCString(true));
    }

    metadata = "Title: " + title + "\n" +
               "Artist: " + artist + "\n" +
               "Album: " + album;
    return true;
}

void PlayerAudio::play()
{
    transportSource.start();
}

void PlayerAudio::pause()
{
    transportSource.stop();
}

void PlayerAudio::mute()
{
    muted = !muted;

    if (muted)
    {
        previousVolume = currentGain;
        setGain(0.0f);
    }
    else
    {
        setGain(previousVolume);
    }
}

void PlayerAudio::setGain(float gain)
{
    currentGain = juce::jlimit(0.0f, 1.0f, gain);
    transportSource.setGain(currentGain);

    if (muted && gain > 0.0f)
    {
        muted = false;
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
    loopingEnabled = shouldloop;
    if (readerSource)
        readerSource->setLooping(shouldloop);
}

void PlayerAudio::setSpeed(float newSpeed)
{
    newSpeed = juce::jlimit(0.25f, 4.0f, newSpeed);

    if (currentSpeed != newSpeed)
    {
        currentSpeed = newSpeed;
        resamplerSource.setResamplingRatio(currentSpeed);
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
                segmentLoopEnabled = false;
                return;
            }
        }
    }

    segmentLoopEnabled = true;
}

void PlayerAudio::clearLoopPoints()
{
    segmentLoopEnabled = false;
    loopStartTime = 0.0;
    loopEndTime = getLength();
}

void PlayerAudio::checkLoopBoundaries()
{
    if (!segmentLoopEnabled || !readerSource)
        return;

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
    fadeInEnabled = true;
}

void PlayerAudio::applyFadeOut()
{
    double totalLength = getLength();
    fadeOutDuration = 0.1 * totalLength;
    fadeOutEnabled = true;
}

void PlayerAudio::removeFades()
{
    fadeInEnabled = false;
    fadeOutEnabled = false;
}

double PlayerAudio::calculateFadeGain(double currentTime) const
{
    double totalLength = getLength();
    double gain = 1.0;

    if (fadeInEnabled && currentTime < fadeInDuration && fadeInDuration > 0.0)
    {
        gain *= (currentTime / fadeInDuration);
    }

    if (fadeOutEnabled && currentTime > (totalLength - fadeOutDuration) && fadeOutDuration > 0.0)
    {
        double fadeOutStart = totalLength - fadeOutDuration;
        double fadeProgress = (currentTime - fadeOutStart) / fadeOutDuration;
        gain *= (1.0 - fadeProgress);
    }

    return juce::jlimit(0.0, 1.0, gain);
}

void PlayerAudio::addMarker(double time, const juce::String& name)
{
    time = juce::jlimit(0.0, getLength(), time);

    juce::String markerName = name;
    if (markerName.isEmpty())
    {
        markerName = "Marker " + juce::String(markers.size() + 1);
    }

    markers.emplace_back(time, markerName);

    std::sort(markers.begin(), markers.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
}

void PlayerAudio::removeMarker(int index)
{
    if (index >= 0 && index < static_cast<int>(markers.size()))
    {
        markers.erase(markers.begin() + index);
    }
}

void PlayerAudio::removeAllMarkers()
{
    markers.clear();
}

void PlayerAudio::jumpToMarker(int index)
{
    if (index >= 0 && index < static_cast<int>(markers.size()))
    {
        setPosition(markers[index].first);
    }
}