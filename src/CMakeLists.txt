add_executable(silv)
target_include_directories(silv PRIVATE ${CMAKE_SOURCE_DIR})

file(GLOB_RECURSE src_f CONFIGURE_DEPENDS
     "*.h"
     "*.c"
)

target_sources(silv PRIVATE
    ${src_f}
)

