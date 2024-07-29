#
# (c) 2024 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software.  Licensee has the right to distribute object form
# only for use with RTI products.  The Software is provided "as is", with no
# warranty of any type, including any warranty for fitness for any purpose.
# RTI is under no obligation to maintain or support the Software.  RTI shall
# not be liable for any incidental or consequential damages arising out of the
# use or inability to use the software.
#

cmake_minimum_required(VERSION 3.12)
project(rticonnnextdds_tsn_sim)

# Add the folder where the FindRTIConnextDDS.cmake file is placed to the
# CMAKE_MODULE_PATH variable
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/cmake/rticonnextdds-cmake-utils/cmake/Modules"
)

include(ConnextDdsCodegen)

find_package(RTIConnextDDS
    "6.1.2"
    REQUIRED
    COMPONENTS
        core
)

include(ConnextDdsGenerateFiles)

connextdds_rtiddsgen_run(
    IDL_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/shapes.idl"
    OUTPUT_DIRECTORY
        "${CMAKE_CURRENT_BINARY_DIR}/src/c++11"
    LANG C++11
    VAR "SHAPE_TYPE"
    DISABLE_PREPROCESSOR
)

connextdds_copy_files(
    INPUT_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/c++11/effector_qos_profiles.xml"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/c++11/surgeon_qos_profiles.xml"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/c++11/video_qos_profiles.xml"
    OUTPUT_DIR
        "${CMAKE_CURRENT_BINARY_DIR}/"
)

# surgeon_console
add_executable(surgeon_console
    "${CMAKE_CURRENT_SOURCE_DIR}/src/c++11/surgeon_console.cxx"
    ${SHAPE_TYPE_CXX11_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/c++11/application.hpp"
    ${SHAPE_TYPE_CXX11_HEADERS}
)
target_link_libraries(surgeon_console
    PRIVATE
        RTIConnextDDS::cpp2_api
)
target_include_directories(surgeon_console
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/src/c++11"
)

# video_server
add_executable(video_server
    "${CMAKE_CURRENT_SOURCE_DIR}/src/c++11/video_server.cxx"
    ${SHAPE_TYPE_CXX11_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/c++11/application.hpp"
    ${SHAPE_TYPE_CXX11_HEADERS}
)
target_link_libraries(video_server
    PRIVATE
        RTIConnextDDS::cpp2_api
)
target_include_directories(video_server
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/src/c++11"
)

# effector_server
add_executable(effector_server
    "${CMAKE_CURRENT_SOURCE_DIR}/src/c++11/effector_server.cxx"
    ${SHAPE_TYPE_CXX11_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/c++11/application.hpp"
    ${SHAPE_TYPE_CXX11_HEADERS}
)
target_link_libraries(effector_server
    PRIVATE
        RTIConnextDDS::cpp2_api
)
target_include_directories(effector_server
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/src/c++11"
)
