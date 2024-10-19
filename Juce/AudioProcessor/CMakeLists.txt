cmake_minimum_required(VERSION 3.15)
project(AudioProcessor VERSION 1.0.0)

# Specify JUCE root directory (adjust path as per your system and cross-platform compatibility)
if (WIN32)
    set(JUCE_DIR C:/Users/Max/JUCE)  # Windows path format
else()
    set(JUCE_DIR /home/Max/JUCE)     # Linux path format
endif()

# Add JUCE as a subdirectory
add_subdirectory(${JUCE_DIR} JUCE)

# Define the shared library
add_library(AudioProcessor SHARED
    Source/GlobalFunction.cpp 
    Source/CompGainEQ.cpp
)

# Link JUCE modules
target_link_libraries(AudioProcessor PRIVATE 
    juce_audio_basics 
    juce_audio_devices 
    juce_audio_formats 
    juce_audio_processors 
    juce_audio_utils 
    juce_core 
    juce_data_structures 
    juce_dsp 
    juce_events 
    juce_graphics 
    juce_gui_basics 
    juce_gui_extra)

# Optionally, enable position-independent code (PIC) for better shared library handling on some platforms
set_target_properties(AudioProcessor PROPERTIES POSITION_INDEPENDENT_CODE ON)