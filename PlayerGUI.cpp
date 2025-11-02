#include "PlayerGUI.h"

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

    if (columnId == 1) // Title
    {
        g.drawText(item.title, 5, 0, width - 5, height, juce::Justification::centredLeft);
    }
    else if (columnId == 2) // Duration
    {
        g.drawText(item.duration, 5, 0, width - 5, height, juce::Justification::centredLeft);
    }
}

juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate)
{
    if (rowNumber >= filteredItems.size()) return existingComponentToUpdate;

    if (columnId == 3 || columnId == 4)  // Load or Remove
    {
        juce::TextButton* button = static_cast<juce::TextButton*>(existingComponentToUpdate);

        if (button == nullptr)
            button = new juce::TextButton();

        if (columnId == 3)  // Load button
        {
            button->setButtonText("Load");
            button->setColour(juce::TextButton::buttonColourId, juce::Colours::blue.withAlpha(0.7f));
        }
        else if (columnId == 4)  // Remove button
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

            if (column == 3 && onTrackLoadRequested) // Load to active track
            {
                onTrackLoadRequested(item.file);
            }
            else if (column == 4) // Remove
            {
                // Find and remove from main list
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
    // Simple implementation - in real app you'd read the file to get duration
    return "0:00";
}


PlayerGUI::PlayerGUI()
{
    playlist = std::make_unique<PlaylistComponent>();
    addAndMakeVisible(playlist.get());

    playlist->onTrackLoadRequested = [this](juce::File file) {  // Removed int parameter
        if (onFileLoaded)
            onFileLoaded(file);
    };

    auto setupModernButton = [this](juce::TextButton& button) {
        button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
        button.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
        button.setColour(juce::TextButton::buttonOnColourId, juce::Colours::white.withAlpha(0.3f));
        button.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        button.setColour(juce::TextButton::textColourOffId, juce::Colours::white.withAlpha(0.9f));
        button.setLookAndFeel(&customLookAndFeel);
    };

    for (auto* btn : { &loadButton, &restartButton, &playpauseButton,
        &skipBackButton, &skipForwardButton, &mute_button, &loopbutton,&gotostartbutton,&gotoendbutton,
        &speedHalfButton, &speedNormalButton, &speedDoubleButton, &speedQuadButton, &setPointAButton, &setPointBButton,
        &clearLoopPointsButton, &toggleSegmentLoopButton, &fadeInButton, &fadeOutButton, &removeFadesButton,  &addMarkerButton, &clearMarkersButton})
    {
        addAndMakeVisible(btn);
        btn->addListener(this);
        setupModernButton(*btn);
    }

    fadeStatusLabel.setText("Fades: None", juce::dontSendNotification);
    fadeStatusLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.9f));
    fadeStatusLabel.setJustificationType(juce::Justification::centredLeft);
    fadeStatusLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    addAndMakeVisible(fadeStatusLabel);
    loopStartLabel.setText("A: --:--", juce::dontSendNotification);
    loopEndLabel.setText("B: --:--", juce::dontSendNotification);
    loopStartLabel.setColour(juce::Label::textColourId, juce::Colours::yellow.withAlpha(0.9f));
    loopEndLabel.setColour(juce::Label::textColourId, juce::Colours::yellow.withAlpha(0.9f));
    loopStartLabel.setJustificationType(juce::Justification::centredLeft);
    loopEndLabel.setJustificationType(juce::Justification::centredRight);
    loopStartLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    loopEndLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    addAndMakeVisible(loopStartLabel);
    addAndMakeVisible(loopEndLabel);
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(volumeSlider);

    volumeLabel.setText("Volume: 50%", juce::dontSendNotification);
    volumeLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.9f));
    volumeLabel.setJustificationType(juce::Justification::centredLeft);
    volumeLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    addAndMakeVisible(volumeLabel);

    speedSlider.setRange(0.25, 4.0, 0.1);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    speedSlider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(speedSlider);

    speedLabel.setText("Speed: 1.0x", juce::dontSendNotification);
    speedLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.9f));
    speedLabel.setJustificationType(juce::Justification::centredLeft);
    speedLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    addAndMakeVisible(speedLabel);

    markersList.setColour(juce::ListBox::backgroundColourId, juce::Colours::transparentBlack);
    markersList.setColour(juce::ListBox::outlineColourId, juce::Colours::white.withAlpha(0.5f));
    markersList.setOutlineThickness(1);
    markersList.setRowHeight(25);
    addAndMakeVisible(markersList);
}

PlayerGUI::~PlayerGUI()
{
    for (auto* btn : { &loadButton, &restartButton, &playpauseButton,
        &skipBackButton, &skipForwardButton, &mute_button, &loopbutton,&gotostartbutton,&gotoendbutton,
        &speedHalfButton, &speedNormalButton, &speedDoubleButton, &speedQuadButton, &addMarkerButton, &clearMarkersButton})

    {
        btn->setLookAndFeel(nullptr);
    }
    volumeSlider.setLookAndFeel(nullptr);
    speedSlider.setLookAndFeel(nullptr);
    playlist.reset();
}

void PlayerGUI::paint(juce::Graphics& g)
{
    auto area = getLocalBounds();

    const int buttonHeight = 35;
    const int rowSpacing = 10;
    const int sliderHeight = 30;
    const int labelHeight = 20;
    const int markersListHeight = 120; // Add height for markers list

    int leftBoxHeight = (4 * buttonHeight) + (3 * rowSpacing) + markersListHeight; // Added 4th row for marker buttons

    int rightBoxHeight = (2 * labelHeight) + (2 * sliderHeight) + (2 * buttonHeight) + (3 * rowSpacing) + 20;

    int controlAreaHeight = juce::jmax(leftBoxHeight, rightBoxHeight);

    auto controlArea = area.removeFromTop(controlAreaHeight + 20);
     auto playlistArea = area.reduced(10);

    auto leftBox = controlArea.removeFromLeft(getWidth() / 2).reduced(5);
    auto rightBox = controlArea.reduced(5);

    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.drawRoundedRectangle(leftBox.toFloat(), 8.0f, 2.0f);
    g.drawRoundedRectangle(rightBox.toFloat(), 8.0f, 2.0f);

    if (!area.isEmpty())
    {
        g.drawRoundedRectangle(area.toFloat(), 8.0f, 2.0f);
    }
}

void PlayerGUI::resized()
{
    auto area = getLocalBounds();

    const int buttonHeight = 35;
    const int rowSpacing = 10;
    const int sliderHeight = 30;
    const int labelHeight = 20;
    const int markersListHeight = 120;

    int leftBoxHeight = (4 * buttonHeight) + (3 * rowSpacing) + markersListHeight;

    int rightBoxHeight = (2 * labelHeight) + (2 * sliderHeight) + (2 * buttonHeight) + (3 * rowSpacing) + 20;

    int controlAreaHeight = juce::jmax(leftBoxHeight, rightBoxHeight);

    auto controlArea = area.removeFromTop(controlAreaHeight + 20);
    auto playlistArea = area.reduced(10);

    auto leftBox = controlArea.removeFromLeft(getWidth() / 2).reduced(10);
    auto rightBox = controlArea.reduced(10);

    auto row1 = leftBox.removeFromTop(buttonHeight);
    int buttonWidth = row1.getWidth() / 5;
    loadButton.setBounds(row1.removeFromLeft(buttonWidth).reduced(2));
    restartButton.setBounds(row1.removeFromLeft(buttonWidth).reduced(2));
    loopbutton.setBounds(row1.removeFromLeft(buttonWidth).reduced(2));
    gotostartbutton.setBounds(row1.removeFromLeft(buttonWidth).reduced(2));
    gotoendbutton.setBounds(row1.removeFromLeft(buttonWidth).reduced(2));

    leftBox.removeFromTop(rowSpacing);

    auto row2 = leftBox.removeFromTop(buttonHeight);
    buttonWidth = row2.getWidth() / 3;
    skipBackButton.setBounds(row2.removeFromLeft(buttonWidth).reduced(2));
    playpauseButton.setBounds(row2.removeFromLeft(buttonWidth).reduced(2));
    skipForwardButton.setBounds(row2.removeFromLeft(buttonWidth).reduced(2));

    leftBox.removeFromTop(rowSpacing);

    auto row3 = leftBox.removeFromTop(buttonHeight);
    int fadeButtonWidth = row3.getWidth() / 4;
    fadeInButton.setBounds(row3.removeFromLeft(fadeButtonWidth).reduced(2));
    fadeOutButton.setBounds(row3.removeFromLeft(fadeButtonWidth).reduced(2));
    removeFadesButton.setBounds(row3.removeFromLeft(fadeButtonWidth).reduced(2));
    fadeStatusLabel.setBounds(row3.reduced(2));

    leftBox.removeFromTop(rowSpacing);

    auto row4 = leftBox.removeFromTop(buttonHeight);
    int markerButtonWidth = row4.getWidth() / 2;
    addMarkerButton.setBounds(row4.removeFromLeft(markerButtonWidth).reduced(2));
    clearMarkersButton.setBounds(row4.removeFromLeft(markerButtonWidth).reduced(2));

    markersList.setBounds(leftBox.reduced(2));

    auto volumeLabelArea = rightBox.removeFromTop(labelHeight);
    volumeLabel.setBounds(volumeLabelArea);

    auto volumeSliderArea = rightBox.removeFromTop(sliderHeight);
    auto muteButtonArea = volumeSliderArea.removeFromRight(80);
    volumeSlider.setBounds(volumeSliderArea.reduced(2));
    mute_button.setBounds(muteButtonArea.reduced(2));

    rightBox.removeFromTop(rowSpacing);

    auto speedLabelArea = rightBox.removeFromTop(labelHeight);
    speedLabel.setBounds(speedLabelArea);

    auto speedSliderArea = rightBox.removeFromTop(sliderHeight);
    speedSlider.setBounds(speedSliderArea.reduced(2));

    auto speedButtonsArea = rightBox.removeFromTop(buttonHeight);
    int speedButtonWidth = speedButtonsArea.getWidth() / 4;
    speedHalfButton.setBounds(speedButtonsArea.removeFromLeft(speedButtonWidth).reduced(2));
    speedNormalButton.setBounds(speedButtonsArea.removeFromLeft(speedButtonWidth).reduced(2));
    speedDoubleButton.setBounds(speedButtonsArea.removeFromLeft(speedButtonWidth).reduced(2));
    speedQuadButton.setBounds(speedButtonsArea.removeFromLeft(speedButtonWidth).reduced(2));

    rightBox.removeFromTop(rowSpacing);

    auto row3Right = rightBox.removeFromTop(buttonHeight);
    int abButtonWidth = row3Right.getWidth() / 4;
    setPointAButton.setBounds(row3Right.removeFromLeft(abButtonWidth).reduced(2));
    setPointBButton.setBounds(row3Right.removeFromLeft(abButtonWidth).reduced(2));
    clearLoopPointsButton.setBounds(row3Right.removeFromLeft(abButtonWidth).reduced(2));
    toggleSegmentLoopButton.setBounds(row3Right.removeFromLeft(abButtonWidth).reduced(2));

    auto loopLabelsArea = rightBox.removeFromTop(20);
    loopStartLabel.setBounds(loopLabelsArea.removeFromLeft(loopLabelsArea.getWidth() / 2).reduced(2));
    loopEndLabel.setBounds(loopLabelsArea.reduced(2));

    playlist->setBounds(playlistArea);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (!connectedPlayer) return;

    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                juce::String metadata;
                if (connectedPlayer->loadfile(file, metadata))
                {
                    if (onFileLoaded)
                        onFileLoaded(file);
                    // Metadata is handled by MainComponent
                }
            });
    }
    else if (button == &restartButton)
    {
        connectedPlayer->setPosition(0.0);
        connectedPlayer->play();
    }
    else if (button == &playpauseButton)
    {
        if (connectedPlayer->isPlaying())
        {
            connectedPlayer->pause();
        }
        else
        {
            connectedPlayer->play();
        }
        updatePlayPauseButton();
    }
    else if (button == &skipBackButton)
    {
        double currentPos = connectedPlayer->getPosition();
        double newPos = currentPos - 10.0;
        if (newPos < 0.0) newPos = 0.0;
        connectedPlayer->setPosition(newPos);
    }
    else if (button == &skipForwardButton)
    {
        double currentPos = connectedPlayer->getPosition();
        double length = connectedPlayer->getLength();
        double newPos = currentPos + 10.0;
        if (newPos > length) newPos = length;
        connectedPlayer->setPosition(newPos);
    }
    else if (button == &mute_button)
    {
        connectedPlayer->mute();
        if (connectedPlayer->getMuteState()) {
            mute_button.setButtonText("Unmute");
            volumeSlider.setValue(0.0);
        } else {
            mute_button.setButtonText("Mute");
            volumeSlider.setValue(connectedPlayer->getPreviousVolume());
        }
    }
    else if (button == &loopbutton)
    {
        bool currentlylooping = connectedPlayer->islooping();
        connectedPlayer->setlooping(!currentlylooping);
        loopbutton.setButtonText(currentlylooping ? "Loop: Off" : "Loop: On");
    }
    else if (button == &gotostartbutton)
    {
        connectedPlayer->setPosition(0.0);
    }

    else if (button == &gotoendbutton)
    {
        double length = connectedPlayer->getLength();
        if (length > 0.0)
        {
            connectedPlayer->setPosition(length);
        }
    }
    else if (button == &speedHalfButton)
    {
        speedSlider.setValue(0.5);
    }
    else if (button == &speedNormalButton)
    {
        speedSlider.setValue(1.0);
    }
    else if (button == &speedDoubleButton)
    {
        speedSlider.setValue(2.0);
    }
    else if (button == &speedQuadButton)
    {
        speedSlider.setValue(4.0);
    }
    else if (button == &setPointAButton)
    {
        double currentPos = connectedPlayer->getPosition();
        // In practice, this will be set by the waveform click in MainComponent
        // For now, just set at current position
        connectedPlayer->setLoopPoints(currentPos, connectedPlayer->getLoopEnd());
        updateLoopLabels();
    }
    else if (button == &setPointBButton)
    {
        double currentPos = connectedPlayer->getPosition();
        connectedPlayer->setLoopPoints(connectedPlayer->getLoopStart(), currentPos);
        updateLoopLabels();
    }
    else if (button == &clearLoopPointsButton)
    {
        connectedPlayer->clearLoopPoints();
        updateLoopLabels();
        toggleSegmentLoopButton.setButtonText("A-B Loop: Off");
    }
    else if (button == &toggleSegmentLoopButton)
    {
        if (connectedPlayer->isSegmentLooping()) {
            connectedPlayer->clearLoopPoints();
            toggleSegmentLoopButton.setButtonText("A-B Loop: Off");
        } else {
            // Enable segment looping if points are set
            if (connectedPlayer->getLoopEnd() > connectedPlayer->getLoopStart()) {
                toggleSegmentLoopButton.setButtonText("A-B Loop: On");
            }
        }
        updateLoopLabels();
    }
    else if (button == &fadeInButton)
    {
        connectedPlayer->applyFadeIn();
        updateFadeStatus();
    }
    else if (button == &fadeOutButton)
    {
        connectedPlayer->applyFadeOut();
        updateFadeStatus();
    }
    else if (button == &removeFadesButton)
    {
        connectedPlayer->removeFades();
        updateFadeStatus();
    }
    else if (button == &addMarkerButton)
    {
        if (connectedPlayer) {
            double currentTime = connectedPlayer->getPosition();
            connectedPlayer->addMarker(currentTime);
            updateMarkersList();
        }
    }
    else if (button == &clearMarkersButton)
    {
        if (connectedPlayer) {
            connectedPlayer->removeAllMarkers();
            updateMarkersList();
        }
    }
}

int PlayerGUI::getNumRows()
{
    return connectedPlayer ? connectedPlayer->getNumMarkers() : 0;
}

void PlayerGUI::paintListBoxItem(int rowNumber, juce::Graphics& g,
                                int width, int height, bool rowIsSelected)
{
    if (rowNumber >= getNumRows()) return;

    auto markers = connectedPlayer->getMarkers();
    if (rowNumber < markers.size()) {
        auto& marker = markers[rowNumber];

        // Format time
        auto formatTime = [](double seconds) {
            int mins = static_cast<int>(seconds) / 60;
            int secs = static_cast<int>(seconds) % 60;
            int ms = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);
            return juce::String::formatted("%02d:%02d.%03d", mins, secs, ms);
        };

        if (rowIsSelected) {
            g.setColour(juce::Colours::white.withAlpha(0.3f));
            g.fillRect(0, 0, width, height);
        }

        g.setColour(juce::Colours::white);
        g.setFont(14.0f);

        juce::String text = formatTime(marker.first) + " - " + marker.second;
        g.drawText(text, 5, 0, width - 5, height, juce::Justification::centredLeft);
    }
}

void PlayerGUI::listBoxItemDoubleClicked(int row, const juce::MouseEvent&)
{
    if (connectedPlayer && row >= 0 && row < connectedPlayer->getNumMarkers()) {
        connectedPlayer->jumpToMarker(row);
    }
}

void PlayerGUI::deleteKeyPressed(int lastRowSelected)
{
    if (connectedPlayer && lastRowSelected >= 0 && lastRowSelected < connectedPlayer->getNumMarkers()) {
        connectedPlayer->removeMarker(lastRowSelected);
        updateMarkersList();
    }
}

void PlayerGUI::updateMarkersList()
{
    markersList.updateContent();
    markersList.repaint();
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (!connectedPlayer) return;

    if (slider == &volumeSlider)
    {
        float volumeValue = static_cast<float>(volumeSlider.getValue());
        connectedPlayer->setGain(volumeValue);
        volumeLabel.setText("Volume: " + juce::String(static_cast<int>(volumeValue * 100)) + "%", juce::dontSendNotification);
    }
    else if (slider == &speedSlider)
    {
        float speedValue = static_cast<float>(speedSlider.getValue());
        connectedPlayer->setSpeed(speedValue);
        speedLabel.setText("Speed: " + juce::String(speedValue, 1) + "x", juce::dontSendNotification);
    }
}

void PlayerGUI::updatePlayPauseButton()
{
    if (!connectedPlayer)
    {
        playpauseButton.setButtonText("> Play");
        return;
    }

    playpauseButton.setButtonText(connectedPlayer->isPlaying() ? "|| Pause" : "> Play");
    playpauseButton.repaint();
}

void PlayerGUI::connectToPlayer(PlayerAudio* player)
{
    connectedPlayer = player;
    updatePlayPauseButton();
}
void PlayerGUI::updateLoopLabels()
{
    if (!connectedPlayer) return;

    auto formatTime = [](double seconds) {
        if (seconds < 0.0) seconds = 0.0;
        int mins = static_cast<int>(seconds) / 60;
        int secs = static_cast<int>(seconds) % 60;
        return juce::String::formatted("%d:%02d", mins, secs);
    };

    loopStartLabel.setText("A: " + formatTime(connectedPlayer->getLoopStart()), juce::dontSendNotification);
    loopEndLabel.setText("B: " + formatTime(connectedPlayer->getLoopEnd()), juce::dontSendNotification);
}
void PlayerGUI::updateFadeStatus()
{
    if (!connectedPlayer) return;

    bool fadeIn = connectedPlayer->hasFadeInApplied();
    bool fadeOut = connectedPlayer->hasFadeOutApplied();

    if (fadeIn && fadeOut) {
        fadeStatusLabel.setText("Fades: Both Applied", juce::dontSendNotification);
    }
    else if (fadeIn) {
        fadeStatusLabel.setText("Fades: In Only", juce::dontSendNotification);
    }
    else if (fadeOut) {
        fadeStatusLabel.setText("Fades: Out Only", juce::dontSendNotification);
    }
    else {
        fadeStatusLabel.setText("Fades: None", juce::dontSendNotification);
    }
}