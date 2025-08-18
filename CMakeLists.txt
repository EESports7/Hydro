cmake_minimum_required(VERSION 3.10.0)

project(libH2O VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create the library target
add_library(libH2O INTERFACE)  # Replace with your actual source file(s)

# Set include directories for the library
target_include_directories(libH2O INTERFACE include)

# Create the executable
add_executable(HydroUtility main.cpp)

# Link the executable with the library
target_link_libraries(HydroUtility PRIVATE libH2O)