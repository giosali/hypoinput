add_executable(${PROJECT_NAME} WIN32)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG e53437b79965e5de9e9bef5a3e7ec609587268d6)
FetchContent_MakeAvailable(cpr)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        cpr::cpr
    LINK_PUBLIC
        lib
)

target_sources(${PROJECT_NAME}
    PRIVATE
        Add-TextExpansion.ps1
        Common.ps1
        Edit-TextExpansions.ps1
        expansions.cpp
        file.cpp
        filesystemwatcher.cpp
        Hypoinput.ico
        Hypoinput.rc
        ini.cpp
        keyboard.cpp
        main.cpp
        Settings.ini
    PUBLIC
        expansions.h
        file.h
        filesystemwatcher.h
        ini.h
        keyboard.h
        main.h
        resource.h
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(${PROJECT_NAME} PUBLIC UNICODE)
target_compile_definitions(${PROJECT_NAME} PUBLIC _UNICODE)
