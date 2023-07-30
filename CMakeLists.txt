cmake_minimum_required(VERSION 3.5)
project(VerifyBot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath='$ORIGIN'")

add_executable(VerifyBot main.cpp
    botcommandhandler.h botcommandhandler.cpp
    botconfig.h botconfig.cpp
    botutil.h botutil.cpp
    commands.h commands.cpp
    constants.h)

include(FetchContent)

# CPR
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git GIT_TAG 2553fc41450301cd09a9271c8d2c3e0cf3546b73)
FetchContent_MakeAvailable(cpr)
target_link_libraries(VerifyBot cpr::cpr)

# DPP
find_package(DPP QUIET)
if(DPP_FOUND)
    target_link_libraries(VerifyBot ${DPP_LIBRARIES})
    target_include_directories(VerifyBot PUBLIC ${DPP_INCLUDE_DIR})
else()
    # get version 10.0.24 (latest as of 7/1/2023)
    option(DPP_BUILD_TEST "" OFF)
    FetchContent_Declare(libdpp GIT_REPOSITORY https://github.com/brainboxdotcc/DPP.git GIT_TAG 63ac3fed8bdd92e3122c6079b944f70fc9546dc2)

    FetchContent_GetProperties(libdpp)
    if(NOT libdpp_POPULATED)
        FetchContent_Populate(libdpp)
        target_include_directories(VerifyBot PUBLIC ${libdpp_SOURCE_DIR}/include)
        add_subdirectory(${libdpp_SOURCE_DIR} ${libdpp_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()

    target_link_libraries(VerifyBot dpp)
endif()

install(TARGETS VerifyBot
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
