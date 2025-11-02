#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
#include "PlaylistComponent.h"

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
        bool shouldDrawButtonAsDown) override;

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;
};

class PlayerGUI : public juce::Component,
                  public juce::Button::Listener,
                  public juce::Slider::Listener,
                  public juce::ListBoxModel
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void resized() override;
    void paint(juce::Graphics& g) override;

    void connectToPlayer(PlayerAudio* player);
    void refreshUI();

    void setOnFileLoadedCallback(std::function<void(juce::File)> callback) {
        onFileLoaded = callback;
    }
    void setMixerCallback(std::function<void(bool, float, float)> callback) {
        onMixerChanged = callback;
    }
    void setMixerPlayPauseCallback(std::function<void()> callback) {
        onMixerPlayPauseRequested = callback;
    }

    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemDoubleClicked(int row, const juce::MouseEvent&) override;
    void deleteKeyPressed(int lastRowSelected) override;

private:
    PlayerAudio* connectedPlayer = nullptr;
    ModernLookAndFeel customLookAndFeel;
    std::unique_ptr<PlaylistComponent> playlist;

    juce::TextButton loadButton{ "Load Files" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton playpauseButton{ "Play" };
    juce::TextButton skipBackButton{ "<<" };
    juce::TextButton skipForwardButton{ ">>" };

    juce::TextButton gotostartbutton{ "Go To Start" };
    juce::TextButton gotoendbutton{ "Go To End" };

    juce::TextButton mute_button{ "Mute" };
    juce::TextButton loopbutton{ "Loop: off" };
    juce::Slider volumeSlider;
    juce::Label volumeLabel;

    juce::Slider speedSlider;
    juce::Label speedLabel;
    juce::TextButton speedHalfButton{ "0.5x" };
    juce::TextButton speedNormalButton{ "1.0x" };
    juce::TextButton speedDoubleButton{ "2.0x" };
    juce::TextButton speedQuadButton{ "4.0x" };

    juce::TextButton setPointAButton{ "Set Point A" };
    juce::TextButton setPointBButton{ "Set Point B" };
    juce::TextButton clearLoopPointsButton{ "Clear A-B" };
    juce::TextButton toggleSegmentLoopButton{ "A-B Loop: Off" };
    juce::Label loopStartLabel;
    juce::Label loopEndLabel;

    juce::TextButton fadeInButton{ "Fade In" };
    juce::TextButton fadeOutButton{ "Fade Out" };
    juce::TextButton removeFadesButton{ "Remove Fades" };
    juce::Label fadeStatusLabel;

    juce::TextButton addMarkerButton{ "Add Marker" };
    juce::TextButton clearMarkersButton{ "Clear Markers" };
    juce::ListBox markersList;

    juce::TextButton mixerToggleButton{ "Mixer: Off" };
    juce::Slider track1MixSlider;
    juce::Slider track2MixSlider;
    juce::Label track1MixLabel;
    juce::Label track2MixLabel;
    bool mixerEnabled = false;

    std::unique_ptr<juce::FileChooser> fileChooser;

    std::function<void(juce::File)> onFileLoaded;
    std::function<void(bool, float, float)> onMixerChanged;
    std::function<void()> onMixerPlayPauseRequested;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    void updatePlayPauseButton();
    void updateLoopLabels();
    void updateFadeStatus();
    void updateMixerControls();
    void updateMarkersList();

    void setupButtons();
    void setupSliders();
    void setupLabels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};