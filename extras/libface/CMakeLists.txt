#
# This file is part of libface.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
#  Created on: Dec 27, 2009
#  @author Alex Jironkin
#  @author Gilles Caulier
#

PROJECT(libface)

#-- Dependencies -----------------------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Doxygen)

#---------------------------------------------------------------------------------------------

SET(${PROJECT_NAME}_MAJOR_VERSION 0)
SET(${PROJECT_NAME}_MINOR_VERSION 1)

SET(LIBFACE_VERSION_STRING "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}")
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

#-- Users Options ----------------------------------------------------------------------------

# Allow the developer to select if Dynamic or Static libraries are built
OPTION (BUILD_SHARED_LIBS "Build Shared Libraries" ON)

SET (LIB_TYPE STATIC)
IF (BUILD_SHARED_LIBS)
    ADD_DEFINITIONS( -DFACE_HAVE_DLL )
    SET (LIB_TYPE SHARED)
ENDIF (BUILD_SHARED_LIBS)

# Allow the developer to compile example programs
OPTION (BUILD_EXAMPLES "Build examples program" OFF)

# Allow the developer to compile example gui program
OPTION (BUILD_EXAMPLES_GUI "Build examples program with GUI" OFF)

OPTION (BUILD_DOCUMENTATION "Build library documentation" OFF)

IF(DOXYGEN_FOUND AND BUILD_DOCUMENTATION)
    SET(API_DIR ${CMAKE_BINARY_DIR}/api)
    SET(SOURCE_DIR ${CMAKE_SOURCE_DIR})
    SET(DOXYFILE ${CMAKE_BINARY_DIR}/Doxyfile)
    SET(WARNFILE ${CMAKE_BINARY_DIR}/doxygen-warn.log)

    CONFIGURE_FILE(Doxyfile.cmake ${DOXYFILE})

    ADD_CUSTOM_TARGET(doc ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
                      WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ENDIF(DOXYGEN_FOUND AND BUILD_DOCUMENTATION)

#---------------------------------------------------------------------------------------------

# add the binary tree to the search path for include files
# so that we will find LibFaceConfig.h

INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src")
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )
MESSAGE(STATUS "OpenCV includes dir: ${OpenCV_INCLUDE_DIRS}")

SET(BINDIR bin)

# Check if system is 32 or 64 bit. Thanks to ulrichard for pointing to link.

IF (NOT APPLE)
    IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
        MESSAGE( STATUS "64 bit architecture detected" )
        SET(LIBDIR lib64)
    ELSE (CMAKE_SIZEOF_VOID_P EQUAL 8)
        MESSAGE( STATUS "32 bit architecture detected" )
        SET(LIBDIR lib)
    ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 8)
ELSE(NOT APPLE)
    MESSAGE( STATUS "Use 32 bit architecture for Apple" )
    SET(LIBDIR lib)
ENDIF(NOT APPLE)

# Check if the user wants to force install into another directory.
# If absolute path is specified the library will be install there,
# If a relative path is specified, this will be prefixed with CMAKE_INSTALL_PREFIX

IF(DEFINED LIB_INSTALL_DIR)
    MESSAGE( STATUS "Forcing to install libface in: ${LIB_INSTALL_DIR} directory" )
    SET(LIBDIR ${LIB_INSTALL_DIR})
ENDIF(DEFINED LIB_INSTALL_DIR)

# Check what build type it is.
#1 and 0 are used in the code so don't change to ON/OFF
IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(DEBUG_MODE 1)
ELSE(CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(DEBUG_MODE 0)
ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug")

# Configure a header file to pass some of the CMake settings
# to the source code

CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/LibFaceConfig.h.cmake" "${PROJECT_BINARY_DIR}/LibFaceConfig.h")

# To prevent warnings from M$ compiler
IF(WIN32 AND MSVC)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32 AND MSVC)

ADD_SUBDIRECTORY(src)
IF(BUILD_EXAMPLES)
    ADD_SUBDIRECTORY(examples)
ENDIF(BUILD_EXAMPLES)

#IF(BUILD_EXAMPLES_GUI)
#    ADD_SUBDIRECTORY(examples/gui)
#ENDIF(BUILD_EXAMPLES_GUI)

# Configure uninstall rules.

CONFIGURE_FILE(cmake/templates/libface_uninstall.cmake ${CMAKE_BINARY_DIR}/cmake_uninstall.cmake COPYONLY)
ADD_CUSTOM_TARGET(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake")

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libface.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libface.pc)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libface.pc DESTINATION ${LIBDIR}/pkgconfig )
INSTALL(FILES ${PROJECT_BINARY_DIR}/LibFaceConfig.h DESTINATION include/${PROJECT_NAME})

# Install find cmake script to the system for client applications.

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/FindLibFace.cmake
        DESTINATION ${CMAKE_ROOT}/Modules)

#---------------------------------------------------------------------------------------------

MESSAGE(STATUS "")
MESSAGE(STATUS "libface ${LIBFACE_VERSION_STRING} configuration: ---------------------------------")
MESSAGE(STATUS "")

IF(DEBUG_MODE)
    MESSAGE(STATUS "DEBUG ------------- YES")
ELSE(DEBUG_MODE)
    MESSAGE(STATUS "DEBUG ------------- NO")
ENDIF(DEBUG_MODE)

IF(BUILD_EXAMPLES)
    IF(BUILD_EXAMPLES_GUI)
        MESSAGE(STATUS "EXAMPLES AND GUI -- YES")
    ELSE(BUILD_EXAMPLES_GUI)
        MESSAGE(STATUS "EXAMPLES ---------- YES")
    ENDIF(BUILD_EXAMPLES_GUI)
ELSE(BUILD_EXAMPLES)
    IF(BUILD_EXAMPLES_GUI)
        MESSAGE(STATUS "GUI --------------- YES")
    ELSE(BUILD_EXAMPLES_GUI)
        MESSAGE(STATUS "GUI --------------- NO")
    ENDIF(BUILD_EXAMPLES_GUI)
    MESSAGE(STATUS "EXAMPLES ---------- NO")
ENDIF(BUILD_EXAMPLES)

IF(BUILD_SHARED_LIBS)
    MESSAGE(STATUS "Build shared lib -- YES")
ELSE(BUILD_SHARED_LIBS)
    MESSAGE(STATUS "Build shared lib -- NO")
ENDIF(BUILD_SHARED_LIBS)

IF(DOXYGEN_FOUND AND BUILD_DOCUMENTATION)
    MESSAGE(STATUS "Documentation ----- YES")
ELSE(DOXYGEN_FOUND AND BUILD_DOCUMENTATION)
    MESSAGE(STATUS "Documentation ----- NO")
ENDIF(DOXYGEN_FOUND AND BUILD_DOCUMENTATION)

MESSAGE(STATUS "")
MESSAGE(STATUS "------------------------------------------------------------")

