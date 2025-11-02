#pragma once
#include <JuceHeader.h>

struct SessionData
{
    juce::File file1;
    juce::File file2;
    double position1 = 0.0;
    double position2 = 0.0;
    bool wasPlaying1 = false;
    bool wasPlaying2 = false;
    int activeTrack = 1;
};

class SessionManager
{
public:
    SessionManager();
    
    void saveSession(const SessionData& data);
    SessionData loadSession();
    
private:
    juce::PropertiesFile* getSettingsFile();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SessionManager)
};