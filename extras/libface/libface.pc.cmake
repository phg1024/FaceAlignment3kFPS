prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=
libdir=${CMAKE_INSTALL_PREFIX}/${LIBDIR}
includedir=${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}

Name: libface
Description: Libface is a C++ library for doing face detection and recognition.
URL: http://libface.sourceforge.net
Requires:
Version: ${LIBFACE_VERSION_STRING}
Libs: -L${CMAKE_INSTALL_PREFIX}/${LIBDIR} -lface
Cflags: -I${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}