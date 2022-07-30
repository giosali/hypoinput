add_library(lib)

target_sources(lib
    PRIVATE
        environment.cpp
        localization.cpp
        utils.cpp
    PUBLIC
        environment.h
        localization.h
        picojson.h
        utils.h
)

target_include_directories(lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(lib PUBLIC UNICODE)
target_compile_definitions(lib PUBLIC _UNICODE)
