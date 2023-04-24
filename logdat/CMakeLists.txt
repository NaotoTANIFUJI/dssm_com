# CMakeのバージョンを設定
cmake_minimum_required(VERSION 3.16)

# プロジェクト名と使用する言語を設定
project(test_viewer_cmake CXX)

if(UNIX)
	include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
	CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
	if(COMPILER_SUPPORTS_CXX11)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	elseif(COMPILER_SUPPORTS_CXX0X)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	else()
		message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Use a different C++ compiler.")
	endif()
endif()

find_package(Eigen3)
IF(NOT EIGEN3_INCLUDE_DIR)
  set(EIGEN3_INCLUDE_DIR $ENV{EIGEN3_ROOT_DIR})
ENDIF() 

include_directories( ../utility/src ${Boost_INCLUDE_DIR} ${EIGEN3_INCLUDE_DIR} )

# set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# set the build type
set(CMAKE_BUILD_TYPE Release)
# confirmation messages
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_C_FLAGS_RELEASE: ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")

add_executable(extra ./src/extradata.cpp ../utility/src/utility.cpp ../utility/src/gmap.cpp)
target_link_libraries(extra m ssm)

add_executable(extra-sa ./src/extradata-sa.cpp ../utility/src/utility.cpp ../utility/src/gmap.cpp)
target_link_libraries(extra-sa m ssm)

#add_executable(extra-yp ./src/extradata-ypspur.cpp ../utility/src/utility.cpp ../utility/src/gmap.cpp)
#target_link_libraries(extra-yp m ssm ypspur)

add_executable(localizer ./src/localizer_test.cpp ../utility/src/utility.cpp ../utility/src/gmap.cpp)
target_link_libraries(localizer m ssm)

#add_executable(remote-localizer ./src/remote-localizer.cpp ../utility/src/utility.cpp ../utility/src/gmap.cpp /home/haselab-08/GitUDPDSSM/utilities/ssm-proxy-client.cpp /home/haselab-08/GitUDPDSSM/utilities/dssm-utility.cpp)
#target_link_libraries(remote-localizer m ssm)





