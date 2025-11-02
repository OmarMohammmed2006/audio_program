#include "WaveformDisplay.h"

WaveformDisplay::WaveformDisplay(juce::AudioThumbnail& thumb, PlayerAudio& player, const juce::String& trackName)
    : thumbnail(thumb), audioPlayer(player), name(trackName)
{
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().reduced(5);
    
    bool highlighted = mixerMode || active;
    g.setColour(highlighted ? juce::Colours::blue.withAlpha(0.4f) : juce::Colours::black.withAlpha(0.3f));
    g.fillRoundedRectangle(area.toFloat(), 8.0f);
    
    g.setColour(mixerMode ? juce::Colours::purple :
                (active ? juce::Colours::yellow : juce::Colours::white.withAlpha(0.5f)));
    g.drawRoundedRectangle(area.toFloat(), 8.0f, 2.0f);
    
    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawText(name, area.removeFromTop(20), juce::Justification::centred);
    
    if (thumbnail.getTotalLength() > 0.0)
    {
        drawWaveform(g);
        drawPlayhead(g, area);
        drawMarkers(g, area);
        drawLoopRegion(g, area);
        drawTimeLabels(g, area);
    }
    else
    {
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        g.drawText("Click to load audio file", area, juce::Justification::centred);
    }
}

void WaveformDisplay::drawWaveform(juce::Graphics& g)
{
    auto area = getLocalBounds().reduced(10, 25);
    bool highlighted = mixerMode || active;
    
    g.setColour(highlighted ? juce::Colours::cyan : juce::Colours::lightblue.withAlpha(0.7f));
    thumbnail.drawChannel(g, area, 0.0, thumbnail.getTotalLength(), 0, 1.0f);
}

void WaveformDisplay::drawPlayhead(juce::Graphics& g, juce::Rectangle<int> area)
{
    auto drawArea = area.reduced(5);
    auto currentPos = audioPlayer.getPosition();
    
    if (thumbnail.getTotalLength() > 0.0)
    {
        auto playheadX = drawArea.getX() + (currentPos / thumbnail.getTotalLength()) * drawArea.getWidth();
        g.setColour(juce::Colours::red);
        g.drawLine(playheadX, drawArea.getY(), playheadX, drawArea.getBottom(), 2.0f);
    }
}

void WaveformDisplay::drawMarkers(juce::Graphics& g, juce::Rectangle<int> area)
{
    if (thumbnail.getTotalLength() <= 0.0 || audioPlayer.getNumMarkers() == 0) 
        return;
    
    auto drawArea = area.reduced(5);
    auto markers = audioPlayer.getMarkers();
    
    for (const auto& marker : markers)
    {
        float markerX = drawArea.getX() + (marker.first / thumbnail.getTotalLength()) * drawArea.getWidth();
        
        g.setColour(juce::Colours::orange);
        g.drawLine(markerX, drawArea.getY(), markerX, drawArea.getBottom(), 1.5f);
        
        g.setColour(juce::Colours::yellow);
        g.fillEllipse(markerX - 3, drawArea.getBottom() - 6, 6, 6);
    }
}

void WaveformDisplay::drawLoopRegion(juce::Graphics& g, juce::Rectangle<int> area)
{
    if (!audioPlayer.isSegmentLooping() || thumbnail.getTotalLength() <= 0.0)
        return;
    
    auto drawArea = area.reduced(5);
    double loopStart = audioPlayer.getLoopStart();
    double loopEnd = audioPlayer.getLoopEnd();
    double totalLength = thumbnail.getTotalLength();
    
    float startX = drawArea.getX() + (loopStart / totalLength) * drawArea.getWidth();
    float endX = drawArea.getX() + (loopEnd / totalLength) * drawArea.getWidth();
    
    g.setColour(juce::Colours::green.withAlpha(0.2f));
    g.fillRect(startX, (float)drawArea.getY(), endX - startX, (float)drawArea.getHeight());
    
    g.setColour(juce::Colours::green);
    g.drawLine(startX, drawArea.getY(), startX, drawArea.getBottom(), 2.0f);
    g.drawLine(endX, drawArea.getY(), endX, drawArea.getBottom(), 2.0f);
    
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("A", startX - 10, drawArea.getY() - 15, 20, 15, juce::Justification::centred);
    g.drawText("B", endX - 10, drawArea.getY() - 15, 20, 15, juce::Justification::centred);
}

void WaveformDisplay::drawTimeLabels(juce::Graphics& g, juce::Rectangle<int> area)
{
    auto drawArea = area.reduced(5);
    auto currentPos = audioPlayer.getPosition();
    
    auto formatTime = [](double seconds) {
        if (seconds < 0.0) seconds = 0.0;
        int mins = static_cast<int>(seconds) / 60;
        int secs = static_cast<int>(seconds) % 60;
        return juce::String::formatted("%d:%02d", mins, secs);
    };
    
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    g.drawText(formatTime(currentPos), drawArea, juce::Justification::bottomLeft);
    g.drawText(formatTime(thumbnail.getTotalLength()), drawArea, juce::Justification::bottomRight);
}

void WaveformDisplay::mouseDown(const juce::MouseEvent& event)
{
    if (onWaveformClicked)
        onWaveformClicked();

    if (thumbnail.getTotalLength() <= 0.0) return;
    
    auto area = getLocalBounds().reduced(10, 25);
    double clickTime = getTimeFromX(event.getPosition().x, area);
    
    audioPlayer.setPosition(clickTime);
    
    if (event.mods.isRightButtonDown())
    {
        audioPlayer.setLoopPoints(clickTime, audioPlayer.getLoopEnd());
    }
    else if (event.mods.isMiddleButtonDown())
    {
        audioPlayer.setLoopPoints(audioPlayer.getLoopStart(), clickTime);
    }
    
    isDragging = true;
    repaint();
}

void WaveformDisplay::mouseDrag(const juce::MouseEvent& event)
{
    if (!isDragging || thumbnail.getTotalLength() <= 0.0) return;
    
    auto area = getLocalBounds().reduced(10, 25);
    double dragTime = getTimeFromX(event.getPosition().x, area);
    
    audioPlayer.setPosition(dragTime);
    repaint();
}

void WaveformDisplay::mouseUp(const juce::MouseEvent&)
{
    isDragging = false;
}

void WaveformDisplay::setActive(bool shouldBeActive)
{
    active = shouldBeActive;
    repaint();
}

void WaveformDisplay::setMixerMode(bool isInMixerMode)
{
    mixerMode = isInMixerMode;
    repaint();
}

double WaveformDisplay::getTimeFromX(int xPosition, juce::Rectangle<int> area) const
{
    double ratio = (xPosition - area.getX()) / (double)area.getWidth();
    ratio = juce::jlimit(0.0, 1.0, ratio);
    return ratio * thumbnail.getTotalLength();
}