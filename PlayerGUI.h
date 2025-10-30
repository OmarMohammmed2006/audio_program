#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class ModernLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModernLookAndFeel()
    {
        setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
        setColour(juce::TextButton::buttonOnColourId, juce::Colours::white.withAlpha(0.3f));
        setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        setColour(juce::TextButton::textColourOffId, juce::Colours::white.withAlpha(0.9f));

        setColour(juce::Slider::trackColourId, juce::Colours::white.withAlpha(0.7f));
        setColour(juce::Slider::thumbColourId, juce::Colours::white);
        setColour(juce::Slider::backgroundColourId, juce::Colours::black.withAlpha(0.3f));
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                             const juce::Colour& backgroundColour,
                             bool shouldDrawButtonAsHighlighted,
                             bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
        const float cornerSize = 12.0f;

        juce::Colour baseColour = backgroundColour;

        if (shouldDrawButtonAsDown)
            baseColour = juce::Colours::white.withAlpha(0.4f);
        else if (shouldDrawButtonAsHighlighted)
            baseColour = juce::Colours::white.withAlpha(0.2f);

        juce::ColourGradient gradient(
            baseColour.brighter(shouldDrawButtonAsHighlighted ? 0.4f : 0.1f),
            bounds.getX(), bounds.getY(),
            baseColour.darker(0.1f),
            bounds.getX(), bounds.getBottom(),
            false
        );

        g.setGradientFill(gradient);
        g.fillRoundedRectangle(bounds, cornerSize);

        g.setColour(juce::Colours::white.withAlpha(0.6f));
        g.drawRoundedRectangle(bounds, cornerSize, 1.5f);
    }

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                     float sliderPos, float minSliderPos, float maxSliderPos,
                     const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        if (style == juce::Slider::LinearHorizontal)
        {
            auto trackHeight = 6.0f;
            auto trackBounds = slider.getLocalBounds().toFloat().reduced(1.0f);
            auto trackY = trackBounds.getCentreY() - trackHeight * 0.5f;

            g.setColour(juce::Colours::black.withAlpha(0.4f));
            g.fillRoundedRectangle(trackBounds.getX(), trackY, trackBounds.getWidth(), trackHeight, trackHeight * 0.5f);

            if (slider.isEnabled())
            {
                g.setColour(juce::Colours::white.withAlpha(0.8f));
                g.fillRoundedRectangle(trackBounds.getX(), trackY, sliderPos - trackBounds.getX(), trackHeight, trackHeight * 0.5f);
            }

            auto thumbSize = 20.0f;
            juce::Point<float> thumbPoint(sliderPos, trackBounds.getCentreY());

            juce::ColourGradient thumbGradient(
                juce::Colours::white.withAlpha(0.9f),
                thumbPoint.x - thumbSize * 0.5f, thumbPoint.y - thumbSize * 0.5f,
                juce::Colours::white.withAlpha(0.6f),
                thumbPoint.x + thumbSize * 0.5f, thumbPoint.y + thumbSize * 0.5f,
                false
            );

            g.setGradientFill(thumbGradient);
            g.fillEllipse(juce::Rectangle<float>(thumbSize, thumbSize).withCentre(thumbPoint));

            g.setColour(juce::Colours::white.withAlpha(0.8f));
            g.drawEllipse(juce::Rectangle<float>(thumbSize, thumbSize).withCentre(thumbPoint), 1.5f);
        }
        else
        {
            LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        }
    }

    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override
    {
        return juce::Font(14.0f, juce::Font::bold);
    }
};

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void resized() override;

    void connectToPlayer(PlayerAudio* player);
    void setOnFileLoadedCallback(std::function<void(juce::File)> callback) {
        onFileLoaded = callback;
    }

private:
    PlayerAudio* connectedPlayer = nullptr;
    ModernLookAndFeel customLookAndFeel;

    juce::TextButton loadButton{ "Load Files" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton playpauseButton{ "Play" };
    juce::TextButton gotostartbutton{ "Go To Start" };
    juce::TextButton gotoendbutton{ "Go To End" };
    juce::TextButton mute_button{ "Mute" };
    juce::TextButton loopbutton{ "Loop: off" };
    juce::TextButton skipBackButton{ "<<" };
    juce::TextButton skipForwardButton{ ">>" };

    juce::TextButton speedHalfButton{ "0.5x" };
    juce::TextButton speedNormalButton{ "1.0x" };
    juce::TextButton speedDoubleButton{ "2.0x" };
    juce::TextButton speedQuadButton{ "4.0x" };

    juce::Slider volumeSlider;
    juce::Slider speedSlider;
    juce::Label volumeLabel;
    juce::Label speedLabel;

    std::unique_ptr<juce::FileChooser> fileChooser;
    std::function<void(juce::File)> onFileLoaded;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void updatePlayPauseButton();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};