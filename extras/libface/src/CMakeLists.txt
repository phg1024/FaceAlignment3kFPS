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

ADD_LIBRARY(face ${LIB_TYPE}
                 LibFaceUtils.cpp
                 FaceDetect.cpp
                 Face.cpp
                 Eigenfaces.cpp
                 LibFace.cpp
                 Haarcascades.cpp)

#SET_TARGET_PROPERTIES(face PROPERTIES COMPILE_FLAGS "-Wall")

SET_TARGET_PROPERTIES(face PROPERTIES LINKER_LANGUAGE "CXX")
SET_TARGET_PROPERTIES(face PROPERTIES OUTPUT_NAME "face")
SET_TARGET_PROPERTIES(face PROPERTIES VERSION ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION})
SET_TARGET_PROPERTIES(face PROPERTIES SOVERSION ${${PROJECT_NAME}_MAJOR_VERSION})
SET_TARGET_PROPERTIES(face PROPERTIES DEFINE_SYMBOL FACE_BUILDING_LIB)

TARGET_LINK_LIBRARIES(face ${OpenCV_LIBS})

INSTALL(TARGETS face
        RUNTIME DESTINATION ${BINDIR}
        LIBRARY DESTINATION ${LIBDIR}
        ARCHIVE DESTINATION ${LIBDIR}
       )

INSTALL(FILES LibFace.h
              Face.h
              LibFaceCore.h
              FaceDetect.h
              Eigenfaces.h
              LibFaceUtils.h
              Haarcascades.h
              DESTINATION include/${PROJECT_NAME})
