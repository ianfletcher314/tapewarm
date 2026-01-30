cmake_minimum_required(VERSION 3.22)

project(TapeWarm VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set JUCE path
set(JUCE_PATH "/Users/ianfletcher/JUCE")

# Add JUCE
add_subdirectory(${JUCE_PATH} ${CMAKE_BINARY_DIR}/JUCE)

# Add the plugin target
juce_add_plugin(TapeWarm
    COMPANY_NAME "Compstortion"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Cmps
    PLUGIN_CODE Tpwm
    FORMATS AU VST3
    PRODUCT_NAME "TapeWarm"
)

target_sources(TapeWarm
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/DSP/TapeProcessor.cpp
)

target_compile_definitions(TapeWarm
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(TapeWarm
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
