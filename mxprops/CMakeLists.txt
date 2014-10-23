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
