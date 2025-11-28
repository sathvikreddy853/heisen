CMAKE_MINIMUM_REQUIRED(VERSION 3.22.1)
PROJECT(mini-heisen LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_POSITION_INDEPENDENT_CODE OFF)


option(CODEGEN "Enable code generation with LLVM" ON)

if(CODEGEN)
    find_package(LLVM REQUIRED CONFIG)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
    
    # Add LLVM definitions
    add_definitions(${LLVM_DEFINITIONS})
    add_definitions(-DENABLE_CODEGEN)
    
    # Include LLVM headers
    include_directories(${LLVM_INCLUDE_DIRS})
endif()

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
SET(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
SET(INC_DIR ${CMAKE_SOURCE_DIR}/include)

FIND_PACKAGE(BISON REQUIRED)
FIND_PACKAGE(FLEX REQUIRED)

BISON_TARGET(Parser
    ${SRC_DIR}/Parse/Parser.y
    ${CMAKE_BINARY_DIR}/Parser.tab.cpp
    DEFINES_FILE ${CMAKE_BINARY_DIR}/Parser.tab.hpp
)

FLEX_TARGET(Lexer
    ${SRC_DIR}/Lex/Lexer.l
    ${CMAKE_BINARY_DIR}/lexer.yy.cpp
)

ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

# Collect all sources first
FILE(GLOB_RECURSE ALL_SOURCES
    "${SRC_DIR}/*.cpp"
)

# Filter sources based on CODEGEN option
if(CODEGEN)
    set(PROJECT_SOURCES ${ALL_SOURCES})
else()
    # Exclude CodeGen sources if CODEGEN is OFF
    foreach(source ${ALL_SOURCES})
        if(NOT source MATCHES ".*/CodeGen/.*")
            list(APPEND PROJECT_SOURCES ${source})
        endif()
    endforeach()
endif()

LIST(APPEND PROJECT_SOURCES
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SOURCES})

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE
    ${INC_DIR}
    ${INC_DIR}/Lex     
    ${INC_DIR}/AST
    ${INC_DIR}/Parse
    ${INC_DIR}/Sema
    ${INC_DIR}/Utils
    ${INC_DIR}/CodeGen
    ${CMAKE_BINARY_DIR}
)

if(CODEGEN)
    # Link against LLVM libraries
    llvm_map_components_to_libnames(llvm_libs core support irreader bitwriter)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${llvm_libs})
endif()

TARGET_COMPILE_OPTIONS(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic
)

# convenience: 'make run' or 'cmake --build . --target run'
ADD_CUSTOM_TARGET(run
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)


enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

file(GLOB_RECURSE TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test/Lex/*.cpp
)

add_executable(lexer_tests
    ${TEST_SOURCES}
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

target_include_directories(lexer_tests PRIVATE
    ${INC_DIR}
    ${INC_DIR}/Lex   
    ${INC_DIR}/AST
    ${INC_DIR}/Parse
    ${INC_DIR}/Sema
    ${INC_DIR}/Utils   
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/test/Lex
)

target_link_libraries(lexer_tests PRIVATE
    GTest::gtest
    GTest::gtest_main
)

gtest_discover_tests(lexer_tests)

