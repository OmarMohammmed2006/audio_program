# 🎵 Audio Player Program

A professional dual-track audio player application built with **JUCE** (Jules Unified Class Explorations) framework and **C++**. This desktop application provides advanced audio playback features including playlist management, real-time waveform visualization, metadata display, audio mixing, and session persistence.

## ✨ Features

### Core Playback Features
- **Dual-Track Audio Engine** - Play and manage two audio tracks simultaneously
- **Advanced Audio Mixing** - Mix audio from two different tracks with independent volume controls
- **Play/Pause Controls** - Full playback control for each track
- **Speed Control** - Adjust playback speed on the fly
- **Volume Control** - Individual gain adjustment for each track
- **Mute/Unmute** - Quick mute toggle for each track
- **Position Seeking** - Jump to any position in the track

### Visual Features
- **Real-Time Waveform Display** - Visual representation of audio tracks with cyan/light blue coloring
- **Playhead Indicator** - Red line showing current playback position
- **Loop Region Visualization** - Green highlighted areas showing loop boundaries with A/B markers
- **Metadata Display** - Show track title, artist, and album information
- **Modern UI Design** - Gradient backgrounds and rounded button corners with hover effects

### Advanced Audio Features
- **Fade In/Out** - Smooth audio fading at track boundaries
- **Segment Looping** - Loop specific regions of audio (A-B loop functionality)
- **Playlist Management** - Add, remove, and search through multiple audio files
- **Audio Metadata Extraction** - Uses TagLib to extract and display ID3 tags
- **Multi-Format Support** - Support for various audio formats via JUCE's format managers

### Session Management
- **Session Persistence** - Automatically saves and loads:
  - Last loaded audio files
  - Playback positions
  - Playing/paused state
  - Currently active track
  - User-defined loop points
  - Volume and speed settings

### User Interface
- **Playlist Component** - Interactive table with search functionality
  - Columns: Title, Duration, Load, Remove
  - Quick search/filter by track name
  - Add files button for easy playlist management
- **Responsive Layout** - Adapts to different window sizes
- **Custom Look and Feel** - Modern gradients and smooth button animations

## 🛠️ System Requirements

### Minimum Requirements
- **OS**: Windows 10 or later (can be adapted for macOS/Linux)
- **RAM**: 4 GB
- **Processor**: Dual-core processor
- **Storage**: 500 MB for application and dependencies

### Development Requirements
- **CMake**: 3.15 or higher
- **Visual Studio**: 2017 or later (with C++ support)
- **C++ Standard**: C++17 or later

## 📥 Installation & Setup

### Prerequisites Installation

#### 1. Install JUCE Framework
```bash
# Clone JUCE repository to C:\ drive
cd C:\
git clone https://github.com/juce-framework/JUCE.git
```

#### 2. Install vcpkg Package Manager
```powershell
# Open PowerShell and navigate to your source directory
cd C:\Users\[YourUsername]\source\repos
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```

#### 3. Install TagLib (for audio metadata extraction)
```powershell
# In vcpkg directory, install TagLib for 64-bit Windows
.\vcpkg.exe install taglib:x64-windows

# Integrate vcpkg with CMake
.\vcpkg.exe integrate install
```

#### 4. Clone TagLib Repository (Optional - for reference)
```bash
# From parent folder of your project
git clone https://github.com/taglib/taglib.git
```

### Building the Project

#### Step 1: Configure CMake
Edit `CMakeLists.txt` and update the following paths to match your system:

```cmake
# Set JUCE path
add_subdirectory("C:/JUCE" JUCE)  # Update to your JUCE installation path

# Set vcpkg triplet (for 64-bit Windows)
set(VCPKG_TARGET_TRIPLET "x64-windows")

# Set TagLib path
set(taglib_DIR "C:/Users/[YourUsername]/Downloads/vcpkg/installed/x64-windows/share/taglib")
```

#### Step 2: Generate Build Files
```bash
# From project root directory
mkdir build
cd build
cmake ..
```

#### Step 3: Compile
```bash
# Using Visual Studio
cmake --build . --config Release

# Or open AudioPlayerApp.sln in Visual Studio and build
```

#### Step 4: Run
```bash
# The executable will be in build/Release or Debug folder
./Release/AudioPlayerApp.exe
```

## 🎮 Usage Guide

### Loading Audio Files
1. Click the **"Add"** button in the playlist section to add audio files
2. Select audio files (supports MP3, WAV, FLAC, OGG, and more)
3. Double-click a track in the playlist to load it
4. Use the **"Load"** button next to each track to load it

### Playing Audio
1. Click the **Play** button (▶) to start playback
2. Click **Pause** (⏸) to pause playback
3. Use the **Progress Slider** to seek to a specific position
4. View real-time playback position on the waveform

### Volume & Mixing
- **Individual Volume Sliders**: Adjust volume for each track independently
- **Mute Button**: Quickly mute/unmute each track
- **Mixer Mode**: Enable mixing to play both tracks simultaneously
- **Track 1 & 2 Volume Controls**: Balance between tracks when mixing

### Loop Features
1. Click on the waveform to set **Loop Start (A)** point
2. Click again to set **Loop End (B)** point
3. Enable **Loop** button to repeat between A and B points
4. Green shaded area shows loop region on waveform

### Playlist Management
1. Use the **Search Box** to filter tracks by name
2. Click **"Load"** to load a specific track
3. Click **"X"** to remove a track from the playlist
4. Add new tracks with the **"Add"** button

### Speed Control
- Use the **Speed Slider** to adjust playback speed (range: 0.5x to 2.0x)

### Metadata Display
- Track metadata (Title, Artist, Album) displays above each waveform
- Updated automatically when loading new audio files
- Extracted from audio file ID3 tags using TagLib

## 📁 Project Structure

```
audio_program/
├── CMakeLists.txt                 # Build configuration
├── Main.cpp                       # Application entry point
├── MainComponent.cpp/h            # Main application window
├── PlayerAudio.cpp/h              # Audio playback engine
├── PlayerGUI.cpp/h                # UI custom styling
├── PlaylistComponent.cpp/h        # Playlist management
├── WaveformDisplay.cpp/h          # Waveform visualization
├── AudioMixer.cpp/h               # Audio mixing logic
├── SessionManager.cpp/h           # Session save/load
├── bg1.jpg, bg2.jpg               # Background images
└── Taglib Install Instructions.txt # Installation guide
```

## 🧩 Component Details

### PlayerAudio
- Handles audio file loading and playback
- Manages transport source and resampler
- Implements fade in/out effects
- Extracts metadata using TagLib
- Controls playback speed and gain

### WaveformDisplay
- Renders audio waveforms in real-time
- Shows playhead position
- Displays loop markers and boundaries
- Supports clicking to set loop points
- Highlights active/inactive tracks

### PlaylistComponent
- TableListBox showing available tracks
- Search/filter functionality
- Load and remove buttons
- Supports drag-and-drop (extensible)

### AudioMixer
- Mixes audio from two independent sources
- Adjustable volume for each track
- Smooth audio blending

### SessionManager
- Saves session data to application settings folder
- Auto-saves every ~5 seconds during playback
- Restores last session on application startup

## 🎛️ Advanced Configuration

### Changing Background Images
Replace `bg1.jpg` and `bg2.jpg` in the project root with your custom images.

### Audio Format Support
Supported formats depend on JUCE's registered format managers:
- MP3, WAV, FLAC, OGG Vorbis, AIFF, and more

### Fade Settings
Customize fade durations in `PlayerAudio.cpp`:
```cpp
const double FADE_IN_TIME = 2.0;   // seconds
const double FADE_OUT_TIME = 2.0;  // seconds
```

## 🐛 Troubleshooting

### "Could not read file" Error
- Ensure the audio file format is supported
- Verify the file is not corrupted
- Check file permissions

### Playback is Silent
- Check volume sliders are not at 0
- Verify system audio is not muted
- Check speaker connections

### Metadata Not Displaying
- File may not have ID3 tags
- TagLib may not be properly linked
- Verify TagLib installation in CMakeLists.txt

### TagLib DLL Not Found
- Ensure `tag.dll` is copied to output directory
- Run: `vcpkg.exe integrate install`
- Verify vcpkg path in CMakeLists.txt

### CMake Cannot Find JUCE
- Update JUCE path in CMakeLists.txt to your installation
- Ensure JUCE folder exists at specified location
- Rebuild after path changes

### Build Fails with "taglib not found"
```powershell
# Reinstall TagLib
cd vcpkg
.\vcpkg.exe install taglib:x64-windows
.\vcpkg.exe integrate install
```

## 🔧 Development

### Building from Source
The project uses modern C++17 features and JUCE audio framework. To modify:

1. **Edit Audio Engine**: Modify `PlayerAudio.cpp`
2. **Change UI**: Edit `PlayerGUI.cpp` and component files
3. **Add Features**: Create new components and link in `MainComponent.cpp`

### Adding New Features
1. Create new `.cpp` and `.h` files
2. Add to `CMakeLists.txt` in `target_sources`
3. Include in `MainComponent.h` and initialize in constructor

## 📜 License

This project uses:
- **JUCE**: GPL v3 License (with commercial options)
- **TagLib**: LGPL License

Ensure compliance with these licenses in your usage.

## 👤 Author

**OmarMohammmed2006**  
GitHub: [@OmarMohammmed2006](https://github.com/OmarMohammmed2006)

## 🤝 Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Open a pull request

## 📞 Support

For issues, questions, or suggestions:
1. Check the Troubleshooting section
2. Review existing issues on GitHub
3. Create a new GitHub issue with detailed information

## 🎯 Future Roadmap

Planned features:
- [ ] Cross-platform support (macOS, Linux)
- [ ] Advanced EQ and effects
- [ ] Visualization presets
- [ ] Keyboard shortcuts customization
- [ ] Theme customization
- [ ] Batch file processing
- [ ] Recording capability
- [ ] Plugin support (VST3)

## 📝 Changelog

### Version 1.0 (Current)
- Initial release
- Dual-track audio playback
- Playlist management
- Waveform visualization
- Session persistence
- Audio mixing
- Loop functionality
- Metadata extraction

---

**Enjoy your music! 🎶**
