# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/miki/Documents/programiki/cpp/picow_ap_class/cmake-build-default/_deps/picotool-src")
  file(MAKE_DIRECTORY "/Users/miki/Documents/programiki/cpp/picow_ap_class/cmake-build-default/_deps/picotool-src")
endif()
file(MAKE_DIRECTORY
  "/Users/miki/Documents/programiki/cpp/picow_ap_class/cmake-build-default/_deps/picotool-build"
  "/Users/miki/Documents/programiki/cpp/picow_ap_class/cmake-build-default/_deps"
  "/Users/miki/Documents/programiki/cpp/picow_ap_class/cmake-build-default/picotool/tmp"
  "/Users/miki/Documents/programiki/cpp/picow_ap_class/cmake-build-default/picotool/src/picotoolBuild-stamp"
  "/Users/miki/Documents/programiki/cpp/picow_ap_class/cmake-build-default/picotool/src"
  "/Users/miki/Documents/programiki/cpp/picow_ap_class/cmake-build-default/picotool/src/picotoolBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/miki/Documents/programiki/cpp/picow_ap_class/cmake-build-default/picotool/src/picotoolBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/miki/Documents/programiki/cpp/picow_ap_class/cmake-build-default/picotool/src/picotoolBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
