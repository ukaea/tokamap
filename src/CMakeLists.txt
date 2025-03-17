find_package( Boost REQUIRED )

set( HANDLER_SRCS
  handlers/mapping_handler.cpp
)

set( MAP_TYPES_SRCS
#  map_types/custom_mapping.cpp
#  map_types/dim_mapping.cpp
#  map_types/expr_mapping.cpp
#  map_types/plugin_mapping.cpp
  map_types/value_mapping.cpp
)

set( UTILS_SRCS
  utils/ram_cache.cpp
  utils/scale_offset.cpp
  utils/subset.cpp
)

add_library( tokamap SHARED library.cpp logging.cpp ${HANDLER_SRCS} ${MAP_TYPES_SRCS} ${UTILS_SRCS} )
target_link_libraries( tokamap PUBLIC Boost::boost )
target_include_directories( tokamap PRIVATE ${CMAKE_SOURCE_DIR}/ext_include ${CMAKE_CURRENT_SOURCE_DIR} )

add_executable( cxx_test test.cpp )
target_link_libraries( cxx_test PRIVATE tokamap )
