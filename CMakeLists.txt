cmake_minimum_required(VERSION 3.20)

project(battery-logger
	VERSION 0.0.1
	DESCRIPTION "Log battery measurements to a log")

find_package(SQLite3 REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(GNUInstallDirs)

add_subdirectory(src)

set(CPACK_GENERATOR "DEB")
string(TOLOWER "${CMAKE_PROJECT_NAME}" lcproject_name)
set(CPACK_DEBIAN_FILE_NAME "${lcproject_name}-${CMAKE_PROJECT_VERSION}.ipk")
set(CPACK_STRIP_FILES TRUE)
if ($ENV{OECORE_TARGET_ARCH} MATCHES "i686")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "qemux86")
else()
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "armv7vehf-neon")
endif()
set(CPACK_PACKAGE_CONTACT beroset@ieee.org)
include(CPack)
