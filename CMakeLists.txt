cmake_minimum_required(VERSION 3.15)
project(AudioPlayer VERSION 0.0.1)


# --- JUCE ---
add_subdirectory("C:/JUCE" JUCE)

set(VCPKG_TARGET_TRIPLET "x64-windows")
set(taglib_DIR "C:/Users/h0610/Downloads/vcpkg/installed/x64-windows/share/taglib")

# --- TagLib (via vcpkg) ---
find_package(taglib CONFIG REQUIRED)   # vcpkg provides this

# --- JUCE App ---
juce_add_gui_app(AudioPlayerApp
		PRODUCT_NAME "Audio Player Program"
)

juce_generate_juce_header(AudioPlayerApp)

# Ensure JUCE headers are in include path
target_include_directories(AudioPlayerApp PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}/AudioPlayerApp/JuceLibraryCode
		${CMAKE_CURRENT_BINARY_DIR}/JuceLibraryCode
)

target_sources(AudioPlayerApp
		PRIVATE
		Main.cpp
		MainComponent.cpp
		MainComponent.h
		PlayerAudio.cpp
		PlayerAudio.h
		PlayerGUI.cpp
		PlayerGUI.h
		PlaylistComponent.cpp
		PlaylistComponent.h
		WaveformDisplay.cpp
		WaveformDisplay.h
		SessionManager.cpp
		SessionManager.h
		AudioMixer.cpp
		AudioMixer.h
)

juce_add_binary_data(AudioPlayerData
		SOURCES
		bg1.jpg
		bg2.jpg
)

target_compile_definitions(AudioPlayerApp
		PRIVATE
		JUCE_WEB_BROWSER=0
		JUCE_USE_CURL=0
		JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:AudioPlayerApp,JUCE_PRODUCT_NAME>"
		JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:AudioPlayerApp,JUCE_VERSION>"
)

target_link_libraries(AudioPlayerApp
		PRIVATE
		AudioPlayerData
		juce::juce_gui_extra
		juce::juce_audio_basics
		juce::juce_audio_formats
		juce::juce_audio_devices
		juce::juce_audio_utils
		juce::juce_dsp
		TagLib::TagLib            # ✅ Link TagLib here

		PUBLIC
		juce::juce_recommended_config_flags
		juce::juce_recommended_lto_flags
		juce::juce_recommended_warning_flags
)

# Copy TagLib DLL to output directory
add_custom_command(TARGET AudioPlayerApp POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"C:/Users/h0610/Downloads/vcpkg/installed/x64-windows/bin/tag.dll"
		$<TARGET_FILE_DIR:AudioPlayerApp>
)