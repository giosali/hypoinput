add_executable(${PROJECT_NAME} WIN32)

target_sources(${PROJECT_NAME}
    PRIVATE
        Add-TextExpansion.ps1
        Common.ps1
        Edit-TextExpansions.ps1
        environment.cpp
        expansions.cpp
        file.cpp
        filesystemwatcher.cpp
        Hypoinput.ico
        Hypoinput.rc
        ini.cpp
        keyboard.cpp
        main.cpp
        Settings.ini
        utils.cpp
    PUBLIC
        environment.h
        expansions.h
        file.h
        filesystemwatcher.h
        ini.h
        keyboard.h
        main.h
        picojson.h
        resource.h
        utils.h
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(${PROJECT_NAME} PUBLIC UNICODE)
target_compile_definitions(${PROJECT_NAME} PUBLIC _UNICODE)
