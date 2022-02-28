# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("depthai_android_jni_example")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(LIB_DIR ${CMAKE_SOURCE_DIR}/main/libs)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/main/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/main/cpp)

include_directories( ${INCLUDE_DIR})
include_directories( ${SRC_DIR})

######         libusb         ######
add_subdirectory("${INCLUDE_DIR}/libusb")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

######         opencv         ######
add_library( opencv-lib SHARED IMPORTED )
set_target_properties(opencv-lib
        PROPERTIES IMPORTED_LOCATION
        ${LIB_DIR}/opencv/${ANDROID_ABI}/libopencv_java4.so)

######         depthai         ######
add_library( depthai-lib SHARED IMPORTED )
set_target_properties(depthai-lib
        PROPERTIES IMPORTED_LOCATION
        ${LIB_DIR}/depthai/${ANDROID_ABI}/libdepthai-core.so)


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        depthai_android_jni_example

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        main/cpp/native-lib.cpp
        main/cpp/utils.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        depthai_android_jni_example

        depthai-lib

        opencv-lib

        usb-1.0

        android

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})