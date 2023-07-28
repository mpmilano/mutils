cmake_minimum_required (VERSION 3.15.4)
project (mutils)
include(GNUInstallDirs)

#Versions
set(mutils_VERSION 1.0)

#CXX FLAGS
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_SHARED_LINKER_FLAGS "-shared --enable-new-dtags")

if ( NOT DEFINED CMAKE_INSTALL_LIBDIR )
    set( CMAKE_INSTALL_LIBDIR lib )
endif ( )

add_library(mutils SHARED utils.cpp abiutils.cpp)
target_include_directories(mutils PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

#Make install
install(TARGETS mutils EXPORT mutilsTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY include/mutils
        DESTINATION include)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/mutils/mutilsConfigVersion.cmake"
  VERSION ${mutils_VERSION}
  COMPATIBILITY AnyNewerVersion
)

set(ConfigPackageLocation lib/cmake/mutils)

configure_package_config_file(mutilsConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/mutils/mutilsConfig.cmake"
    INSTALL_DESTINATION ${ConfigPackageLocation}
    PATH_VARS CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_INCLUDEDIR ConfigPackageLocation
)

install(EXPORT mutilsTargets
    FILE mutilsTargets.cmake
    NAMESPACE mutils::
    DESTINATION ${ConfigPackageLocation})

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/mutils/mutilsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/mutils/mutilsConfigVersion.cmake"
  DESTINATION ${ConfigPackageLocation}
)
