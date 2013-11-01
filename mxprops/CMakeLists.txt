project(mxprops)

find_boost_libs(thread system)

add_library(mxprops SHARED
  mxprops.h
  pathprop.h
  src/mxprops.cpp
)

target_link_libraries(mxprops
  ${Boost_LIBRARIES}
)

