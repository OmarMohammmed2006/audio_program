#include "SessionManager.h"

SessionManager::SessionManager() = default;

void SessionManager::saveSession(const SessionData& data)
{
    std::unique_ptr<juce::PropertiesFile> settings(getSettingsFile());
    
    settings->setValue("lastFile1", data.file1.getFullPathName());
    settings->setValue("lastFile2", data.file2.getFullPathName());
    settings->setValue("lastPosition1", data.position1);
    settings->setValue("lastPosition2", data.position2);
    settings->setValue("wasPlaying1", data.wasPlaying1);
    settings->setValue("wasPlaying2", data.wasPlaying2);
    settings->setValue("lastActiveTrack", data.activeTrack);
    
    settings->save();
}

SessionData SessionManager::loadSession()
{
    SessionData data;
    std::unique_ptr<juce::PropertiesFile> settings(getSettingsFile());
    
    if (!settings->getFile().existsAsFile())
        return data;
    
    juce::String path1 = settings->getValue("lastFile1");
    juce::String path2 = settings->getValue("lastFile2");
    
    if (juce::File::isAbsolutePath(path1))
        data.file1 = juce::File(path1);
    
    if (juce::File::isAbsolutePath(path2))
        data.file2 = juce::File(path2);
    
    data.position1 = settings->getDoubleValue("lastPosition1", 0.0);
    data.position2 = settings->getDoubleValue("lastPosition2", 0.0);
    data.wasPlaying1 = settings->getBoolValue("wasPlaying1", false);
    data.wasPlaying2 = settings->getBoolValue("wasPlaying2", false);
    data.activeTrack = settings->getIntValue("lastActiveTrack", 1);
    
    return data;
}

juce::PropertiesFile* SessionManager::getSettingsFile()
{
    juce::PropertiesFile::Options options;
    options.applicationName = "AudioPlayer";
    options.filenameSuffix = "settings";
    options.osxLibrarySubFolder = "Application Support";
    
    juce::File settingsFolder = juce::File::getSpecialLocation(
        juce::File::userApplicationDataDirectory).getChildFile("AudioPlayer");
    options.folderName = settingsFolder.getFullPathName();
    
    return new juce::PropertiesFile(options);
}