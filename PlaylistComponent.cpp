#include "PlaylistComponent.h"

PlaylistComponent::PlaylistComponent()
{
    addAndMakeVisible(table);
    addAndMakeVisible(searchBox);
    addAndMakeVisible(addButton);

    table.setModel(this);
    table.getHeader().addColumn("Title", 1, 300);
    table.getHeader().addColumn("Duration", 2, 100);
    table.getHeader().addColumn("Load", 3, 100);
    table.getHeader().addColumn("Remove", 4, 100);

    table.setAutoSizeMenuOptionShown(true);
    table.getHeader().setStretchToFitActive(true);

    searchBox.setTextToShowWhenEmpty("Search playlist...", juce::Colours::white.withAlpha(0.5f));
    searchBox.addListener(this);

    addButton.addListener(this);
    addButton.setTooltip("Add files to playlist");
}

void PlaylistComponent::resized()
{
    auto area = getLocalBounds();
    auto searchArea = area.removeFromTop(30);

    searchBox.setBounds(searchArea.removeFromLeft(area.getWidth() - 40).reduced(2));
    addButton.setBounds(searchArea.reduced(2));

    table.setBounds(area);
}

int PlaylistComponent::getNumRows()
{
    return filteredItems.size();
}

void PlaylistComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    auto colour = rowIsSelected ? juce::Colours::lightblue.withAlpha(0.3f)
        : (rowNumber % 2 == 0 ? juce::Colours::white.withAlpha(0.1f)
            : juce::Colours::white.withAlpha(0.05f));
    g.setColour(colour);
    g.fillRect(0, 0, width, height);
}

void PlaylistComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    if (rowNumber >= filteredItems.size()) return;

    auto& item = filteredItems[rowNumber];

    g.setColour(juce::Colours::white);
    g.setFont(14.0f);

    if (columnId == 1)
    {
        g.drawText(item.title, 5, 0, width - 5, height, juce::Justification::centredLeft);
    }
    else if (columnId == 2)
    {
        g.drawText(item.duration, 5, 0, width - 5, height, juce::Justification::centredLeft);
    }
}

juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate)
{
    if (rowNumber >= filteredItems.size()) return existingComponentToUpdate;

    if (columnId == 3 || columnId == 4)
    {
        juce::TextButton* button = static_cast<juce::TextButton*>(existingComponentToUpdate);

        if (button == nullptr)
            button = new juce::TextButton();

        if (columnId == 3)
        {
            button->setButtonText("Load");
            button->setColour(juce::TextButton::buttonColourId, juce::Colours::blue.withAlpha(0.7f));
        }
        else if (columnId == 4)
        {
            button->setButtonText("X");
            button->setColour(juce::TextButton::buttonColourId, juce::Colours::red.withAlpha(0.7f));
        }

        button->addListener(this);
        button->setComponentID(juce::String(rowNumber) + "_" + juce::String(columnId));

        return button;
    }

    return existingComponentToUpdate;
}

void PlaylistComponent::buttonClicked(juce::Button* button)
{
    auto id = button->getComponentID();
    auto parts = juce::StringArray::fromTokens(id, "_", "");

    if (parts.size() == 2)
    {
        int row = parts[0].getIntValue();
        int column = parts[1].getIntValue();

        if (row < filteredItems.size())
        {
            auto& item = filteredItems[row];

            if (column == 3 && onTrackLoadRequested)
            {
                onTrackLoadRequested(item.file);
            }
            else if (column == 4)
            {
                auto it = std::find_if(playlistItems.begin(), playlistItems.end(),
                    [&](const PlaylistItem& playlistItem) { return playlistItem.file == item.file; });

                if (it != playlistItems.end())
                {
                    playlistItems.erase(it);
                    updateFilter();
                    table.updateContent();
                }
            }
        }
    }
    else if (button == &addButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>("Select audio files...", juce::File{}, "*.wav;*.mp3;*.aiff");

        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& fc)
            {
                auto results = fc.getResults();
                for (auto& file : results)
                {
                    addFileToPlaylist(file);
                }
                table.updateContent();
            });
    }
}

void PlaylistComponent::textEditorReturnKeyPressed(juce::TextEditor& editor)
{
    updateFilter();
    table.updateContent();
}

void PlaylistComponent::addFileToPlaylist(const juce::File& file)
{
    PlaylistItem item;
    item.file = file;
    item.title = file.getFileNameWithoutExtension();
    item.duration = getDurationString(file);

    playlistItems.push_back(item);
    updateFilter();
}

void PlaylistComponent::clearPlaylist()
{
    playlistItems.clear();
    updateFilter();
    table.updateContent();
}

void PlaylistComponent::updateFilter()
{
    auto searchText = searchBox.getText().toLowerCase();

    if (searchText.isEmpty())
    {
        filteredItems = playlistItems;
    }
    else
    {
        filteredItems.clear();
        for (auto& item : playlistItems)
        {
            if (item.title.toLowerCase().contains(searchText))
            {
                filteredItems.push_back(item);
            }
        }
    }
}

juce::String PlaylistComponent::getDurationString(const juce::File& file)
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader != nullptr)
    {
        double lengthInSeconds = reader->lengthInSamples / reader->sampleRate;

        int minutes = static_cast<int>(lengthInSeconds) / 60;
        int seconds = static_cast<int>(lengthInSeconds) % 60;

        return juce::String::formatted("%d:%02d", minutes, seconds);
    }

    return "0:00";
}