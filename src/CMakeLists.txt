
INCLUDE_DIRECTORIES(${PC_METLIBS_INCLUDE_DIRS})
ADD_DEFINITIONS(${PC_METLIBS_CFLAGS_OTHER})
LINK_DIRECTORIES(${PC_METLIBS_LIBRARY_DIRS})

SET(common_SOURCES
  CoServer4.cc
  CoSocket.cc
)

SET(coserver4_SOURCES
  main.cc
)

SET(coserver4_stop_SOURCES 
  coserver4_stop.cc
)

########################################

ADD_LIBRARY(libco4server STATIC
  ${common_SOURCES}
)

TARGET_LINK_LIBRARIES(libco4server
  ${PC_METLIBS_LIBRARIES}
  Qt5::Network Qt5::Core
)

ADD_EXECUTABLE(coserver4
  ${coserver4_SOURCES}
)

TARGET_LINK_LIBRARIES(coserver4
  libco4server
)

ADD_EXECUTABLE(coserver4_stop
  ${coserver4_stop_SOURCES}
)

TARGET_LINK_LIBRARIES(coserver4_stop
  libco4server
)

########################################

INSTALL(TARGETS coserver4 coserver4_stop
  DESTINATION ${CMAKE_INSTALL_BINDIR}
)
