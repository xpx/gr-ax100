INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_AX100 ax100)

FIND_PATH(
    AX100_INCLUDE_DIRS
    NAMES ax100/api.h
    HINTS $ENV{AX100_DIR}/include
        ${PC_AX100_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    AX100_LIBRARIES
    NAMES gnuradio-ax100
    HINTS $ENV{AX100_DIR}/lib
        ${PC_AX100_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(AX100 DEFAULT_MSG AX100_LIBRARIES AX100_INCLUDE_DIRS)
MARK_AS_ADVANCED(AX100_LIBRARIES AX100_INCLUDE_DIRS)

