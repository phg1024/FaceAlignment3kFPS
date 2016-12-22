#
# This file was adopted using example from http://qtnode.net/wiki/Qt4_with_cmake
#

PROJECT(libfaceGUI) # the name of your project

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(Qt4 REQUIRED) # find and setup Qt4 for this project

# tell cmake to process CMakeLists.txt in that subdirectory
ADD_SUBDIRECTORY(src)