# - Try to find log4cplus
# Once done, this will define
#
#  log4cplus_FOUND - system has log4cplus
#  log4cplus_INCLUDE_DIRS - the log4cplus include directories
#  log4cplus_LIBRARIES - link these to use log4cplus

#libfind_pkg_check_modules(log4cplus_PKGCONF log4cplus)

include(LibFindMacros)

# Include dir
find_path(log4cplus_INCLUDE_DIR
  NAMES config.h
  HINTS "/usr/include"
        "/usr/include/log4cpp"
        "/usr/local/include"
        "/usr/local/include/log4cpp"
        "/opt/local/include"
        "/opt/local/include/log4cpp"
        ${log4cplus_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(log4cplus_LIB
  NAMES log4cpp liblog4cpp liblog4cpp-static
  HINTS "/usr/lib"
        "/usr/local/lib"
        "/opt/local/lib"
        ${log4cplus_PKGCONF_LIBRARY_DIRS}
)

set(log4cplus_PROCESS_INCLUDES log4cplus_INCLUDE_DIR)
set(log4cplus_PROCESS_LIBS log4cplus_LIB)

libfind_process(log4cplus)
