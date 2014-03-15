project(mxprops)

find_boost_libs(thread system)

add_library(mxprops SHARED
  mxprops.h
  io.h
  pathprop.h
  src/mxprops.cpp
  src/io.cpp
)

target_link_libraries(mxprops
  ${Boost_LIBRARIES}
  jsoncpp
)
