
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src" ${PC_METLIBS_INCLUDE_DIRS})
ADD_DEFINITIONS(
  ${PC_METLIBS_CFLAGS_OTHER}
  -DTEST_SRCDIR="${CMAKE_CURRENT_SOURCE_DIR}/"
  -DTEST_BUILDDIR="${CMAKE_CURRENT_BINARY_DIR}/"
)
LINK_DIRECTORIES(${PC_METLIBS_LIBRARY_DIRS})

SET(coserver4Test_SOURCES
  TestHelpers.h
  TestHelpers.cc
  Co4Test1.cc
  Co4Test2.cc
  Co4Test3.cc
  Co4Test4.cc
  gtestMainQt.cc
)

ADD_EXECUTABLE(coserver4Test
  ${coserver4Test_SOURCES}
)

TARGET_LINK_LIBRARIES(coserver4Test
  libco4server
  ${GTEST_LIBRARY}
  ${GTEST_MAIN_LIBRARY}
)

ADD_TEST(NAME coserver4Test
  COMMAND coserver4Test --gtest_color=yes
)
