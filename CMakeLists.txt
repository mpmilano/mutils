cmake_minimum_required (VERSION 3.1)
project (mutils)

#Versions
set(mutils_VERSION 1.0)

#CXX FLAGS
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++14 -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++14 -fPIC -g")
set(CMAKE_SHARED_LINKER_FLAGS "-shared --enable-new-dtags")

add_library(mutils SHARED utils.cpp abiutils.cpp)
target_include_directories(mutils PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

#Make install
install(TARGETS mutils EXPORT mutils
        LIBRARY DESTINATION lib)
install(DIRECTORY include/mutils
        DESTINATION include)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/mutils/mutilsConfigVersion.cmake"
  VERSION ${mutils_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export (EXPORT mutils
  FILE "${CMAKE_CURRENT_BINARY_DIR}/mutils/mutilsTargets.cmake"
)

configure_file (mutilsConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/mutils/mutilsConfig.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/cmake/mutils)
install(EXPORT mutils
  FILE mutilsTargets.cmake
  DESTINATION ${ConfigPackageLocation}
)

install(
  FILES
    mutilsConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/mutils/mutilsConfigVersion.cmake"
  DESTINATION ${ConfigPackageLocation}
)
