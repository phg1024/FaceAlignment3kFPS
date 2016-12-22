#
# This file was adopted using example from http://qtnode.net/wiki/Qt4_with_cmake
#

INCLUDE(${QT_USE_FILE})

SET(libfaceGUI_UIS
    mainwindow.ui
)

QT4_WRAP_UI(libfaceGUI_UIS_H ${libfaceGUI_UIS})

INCLUDE_DIRECTORIES(
   ${CMAKE_CURRENT_BINARY_DIR}
)

SET(libfaceGUI_SRCS
    faceitem.cpp
    main.cpp
    mainwindow.cpp
)

SET(libfaceGUI_MOC_HDRS
  mainwindow.h
  faceitem.h
)

QT4_WRAP_CPP(libfaceGUI_MOC_SRCS ${libfaceGUI_MOC_HDRS})

QT4_AUTOMOC(${libfaceGUI_SRCS})

ADD_EXECUTABLE(libfaceGUI ${libfaceGUI_SRCS} ${libfaceGUI_MOC_SRCS} ${libfaceGUI_UIS_H})

TARGET_LINK_LIBRARIES(libfaceGUI face ${QT_LIBRARIES})