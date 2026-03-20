cmake_minimum_required(VERSION 3.7)
project(tyshi-engine)

set(engine_name tyshi-engine)

file(GLOB_RECURSE ENGINE_SOURCES src/*.cpp src/*.c)

add_library(${engine_name} STATIC ${ENGINE_SOURCES})

target_include_directories(${engine_name}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(${engine_name} PUBLIC cxx_std_20)

execute_process(
    COMMAND git rev-parse --short=12 HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE ENGINE_GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

if(ENGINE_GIT_HASH STREQUAL "")
    set(ENGINE_GIT_HASH "unknown")
endif()

target_compile_definitions(${engine_name} PUBLIC
    $<$<CONFIG:Debug>:DEBUG_BUILD>
    $<$<CONFIG:Release>:RELEASE_BUILD>
    ENGINE_VERSION="${ENGINE_GIT_HASH}"
)

if(CMAKE_TOOLCHAIN_FILE)
    message(STATUS "tyshi-engine: Building for Windows (cross-compile)")
else()
    message(STATUS "tyshi-engine: Building for Linux/Default")
endif()

find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL3       REQUIRED sdl3)
pkg_check_modules(SDL3_IMAGE REQUIRED sdl3-image)
pkg_check_modules(SDL3_TTF   REQUIRED sdl3-ttf)

target_include_directories(${engine_name} PUBLIC
    ${SDL3_INCLUDE_DIRS}
    ${SDL3_IMAGE_INCLUDE_DIRS}
    ${SDL3_TTF_INCLUDE_DIRS}
)

target_link_directories(${engine_name} PUBLIC
    ${SDL3_LIBRARY_DIRS}
    ${SDL3_IMAGE_LIBRARY_DIRS}
    ${SDL3_TTF_LIBRARY_DIRS}
)

target_link_libraries(${engine_name} PUBLIC
    ${SDL3_LIBRARIES}
    ${SDL3_IMAGE_LIBRARIES}
    ${SDL3_TTF_LIBRARIES}
)

target_link_options(${engine_name} PUBLIC
    ${SDL3_LDFLAGS_OTHER}
    ${SDL3_IMAGE_LDFLAGS_OTHER}
    ${SDL3_TTF_LDFLAGS_OTHER}
)

target_compile_options(${engine_name} PUBLIC
    ${SDL3_CFLAGS_OTHER}
    ${SDL3_IMAGE_CFLAGS_OTHER}
    ${SDL3_TTF_CFLAGS_OTHER}
)


if(CMAKE_TOOLCHAIN_FILE)
    target_link_libraries(${engine_name} PUBLIC -static-libstdc++ -static-libgcc)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_options(${engine_name} PUBLIC -mconsole)
    else()
        target_link_options(${engine_name} PUBLIC -mwindows)
    endif()
endif()



if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    message(STATUS "tyshi-engine: Building standalone with test app")

    file(GLOB TEST_SOURCES test/*.cpp test/*.h)

    if(CMAKE_TOOLCHAIN_FILE)
    execute_process(
        COMMAND pkg-config --variable=prefix sdl3
        OUTPUT_VARIABLE SDL3_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    install(FILES
        ${SDL3_PREFIX}/bin/SDL3.dll
        ${SDL3_PREFIX}/bin/SDL3_image.dll
        ${SDL3_PREFIX}/bin/SDL3_ttf.dll
        DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
    else()
        set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/bin/linux" CACHE PATH "" FORCE)
    endif()

    add_executable(tyshi-engine-test ${TEST_SOURCES})
    target_link_libraries(tyshi-engine-test PRIVATE ${engine_name})

    install(TARGETS tyshi-engine-test RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/engine/assets
)