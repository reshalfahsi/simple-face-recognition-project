#####################################
# Define Project-Wide Settings
#####################################
cmake_minimum_required(VERSION 3.10.0)
project(face-recognition-project LANGUAGES CXX  VERSION 1.0.0 )
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SKIP_BUILD_RPATH OFF)
set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

#####################################
# Define Conan Imports
#####################################
include(GenerateExportHeader)

#####################################
# Set compiler specific settings
#####################################

list(APPEND compiler_flags
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<CXX_COMPILER_ID:MSVC>:/WX>
  $<$<CXX_COMPILER_ID:MSVC>:/GL>
  $<$<CXX_COMPILER_ID:MSVC>:/Yu>
  $<$<CXX_COMPILER_ID:MSVC>:/O2>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:CLANG>>:-Wall>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:CLANG>>:-Wextra>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:CLANG>>:-Werror=format-security>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:CLANG>>:-Werror=unused-result>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:CLANG>>:-fasynchronous-unwind-tables>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:CLANG>>:-fexceptions>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:CLANG>>:-pipe>
)

#####################################
# Define Targets
#####################################
include_directories(src)
add_subdirectory(dlib)

set(OpenCV_DIR "/usr/local/lib/opencv4" CACHE PATH '' ${SHOULD_FORCE_CACHE})
find_package(OpenCV REQUIRED CONFIG)

set(PROJECT_TARGETS)
add_executable(FaceRecognition src/main.cpp)

add_library(facerecognitionlib src/FaceRecognition/FaceRecognition.cpp)

target_link_libraries(facerecognitionlib
       pthread
       ${OpenCV_LIBS}
       dlib
)

set_target_properties(facerecognitionlib
    PROPERTIES
      VERSION ${PROJECT_VERSION}
      PUBLIC_HEADER "src/FaceRecognition/FaceRecognition.h"
      POSITION_INDEPENDENT_CODE ON
)

target_compile_options(facerecognitionlib PRIVATE ${compiler_flags})

target_link_libraries(FaceRecognition 
       facerecognitionlib
       pthread
       ${OpenCV_LIBS}
)

#####################################
# Define Install Targets
#####################################

install(TARGETS
  facerecognitionlib
  FaceRecognition
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include
)
