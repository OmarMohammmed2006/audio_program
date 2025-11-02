#pragma once
#include <JuceHeader.h>

class PlaylistComponent : public juce::Component,
                          public juce::TableListBoxModel,
                          public juce::Button::Listener,
                          public juce::TextEditor::Listener
{
public:
    PlaylistComponent();
    ~PlaylistComponent() override = default;

    void resized() override;

    int getNumRows() override;
    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;

    void buttonClicked(juce::Button* button) override;
    
    void textEditorReturnKeyPressed(juce::TextEditor& editor) override;

    void addFileToPlaylist(const juce::File& file);
    void clearPlaylist();
    std::vector<juce::File> getAllFiles() const;
    
    std::function<void(juce::File)> onTrackLoadRequested;

private:
    struct PlaylistItem
    {
        juce::File file;
        juce::String title;
        juce::String duration;
    };

    juce::TableListBox table;
    juce::TextEditor searchBox;
    juce::TextButton addButton{ "+" };

    std::vector<PlaylistItem> playlistItems;
    std::vector<PlaylistItem> filteredItems;
    std::unique_ptr<juce::FileChooser> fileChooser;

    void updateFilter();
    juce::String getDurationString(const juce::File& file);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistComponent)
};