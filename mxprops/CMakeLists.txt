project(mxprops)

find_boost_libs(thread system)

add_library(mxprops STATIC
  mxprops.h
  pathprop.h
  io.h
  src/io.cpp
)

target_link_libraries(mxprops
  ${Boost_LIBRARIES}
)

option(MXPROPS_WITH_TESTS "Enable testing with GTest and CTest" ON)
if (MXPROPS_WITH_TESTS)
  add_executable(mxprops_test
  	test/mxprops_test.cpp)
  target_link_libraries(mxprops_test
  	mxprops
  	gtest)
  add_test(mxprops_test ${COMMON_RUNTIME_OUTPUT_DIRECTORY}/mxprops_test)
endif()
