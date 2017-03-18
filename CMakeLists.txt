cmake_minimum_required(VERSION 3.6)
project(Islang)

set(CMAKE_CXX_FLAGS "-std=gnu++14 -Wall")
message(STATUS "Please show me this shit: " ${CMAKE_BUILD_TYPE})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Debug mode")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    add_definitions(-DDEBUG)
else()
    message("Release mode")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++ -O3")
endif()

if(WIN32)
    message("Windowsing too!")
    find_library(SHLWAPI Shlwapi.lib)
else(WIN32)
    set(SHLWAPI)
endif(WIN32)

link_libraries(SHLWAPI)

set(SOURCE_FILES src/main.cpp src/Lexer.cpp src/Lexer.h src/Token.cpp src/Token.h src/Scanner.cpp src/Scanner.h src/Character.cpp src/Character.h src/IslangException.cpp src/IslangException.h src/Grammar.cpp src/Grammar.h src/Color.h src/Color.cpp src/Parser.cpp src/Parser.h src/AST.cpp src/AST.h src/Variable.cpp src/Variable.h src/ParserPromise.cpp src/ParserPromise.h src/TokenException.cpp src/TokenException.h src/ParserException.cpp src/ParserException.h src/ParserError.cpp src/ParserError.h src/Scope.cpp src/Scope.h src/Interpreter.cpp src/Interpreter.h src/InterpreterException.cpp src/InterpreterException.h src/InterpreterError.cpp src/InterpreterError.h src/CLI.cpp src/CLI.h src/Compat.h src/Compat.cpp)
add_executable(Islang ${SOURCE_FILES})