cmake_minimum_required(VERSION 3.10)
project(ScreenStreamer)

set(CMAKE_CXX_STANDARD 17)

# Add executable
add_executable(streamer main.cpp)

# Link Windows libraries
target_link_libraries(streamer 
    winhttp
    gdiplus
)

# Set Windows subsystem
if(WIN32)
    set_target_properties(streamer PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()
