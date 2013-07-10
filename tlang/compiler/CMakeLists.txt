#cmake file for buiding tlang-compiler package
#cmake compability
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.4)

#allow more human readalbe "if then else" construct
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

# cmake project name, default language is cxx
PROJECT(TCC CXX)

#project version
SET ($(PROJECT_NAME)_MAJOR_VERSION 0)
SET ($(PROJECT_NAME)_MINOR_VERSION 1)
SET ($(PROJECT_NAME)_PATCH_LEVEL 0)
SET (TARGET_NAME tcc)

#project option
IF (NOT CMAKE_BUILD_TYPE)
    SET (CMAKE_BUILD_TYPE Debug)
ENDIF()


#include directoris
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/Include")

# c++ definitions
# ADD_DEFINITIONS("-Wall")
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS("-DDEBUG")
ENDIF()


#for all subdirs, collect all files
# Grammar directory
INCLUDE (Grammar/CMakeLists.txt)
FOREACH (FILE ${FILES})
    SET (grammarDirFiles ${gramarDirFiles} Grammar/${FILE})
ENDFOREACH()
# lexer directory
INCLUDE (Lexer/CMakeLists.txt)
FOREACH (FILE ${FILES})
    SET (lexerDirFiles ${lexerDirFiles} Lexer/${FILE})
ENDFOREACH()
# AST directory
INCLUDE (AST/CMakeLists.txt)
FOREACH (FILE ${FILES})
    SET (astDirFiles ${astDirFiles} AST/${FILE})
ENDFOREACH()
# parser directory
INCLUDE (Parser/CMakeLists.txt)
FOREACH (FILE ${FILES})
    SET (parserDirFiles ${parserDirFiles} Parser/${FILE})
ENDFOREACH()
# runtime directory
INCLUDE (Runtime/CMakeLists.txt)
FOREACH (FILE ${FILES})
    SET (runtimeDirFiles ${runtimeDirFiles} Runtime/${FILE})
ENDFOREACH()
# type directory
INCLUDE (Type/CMakeLists.txt)
FOREACH (FILE ${FILES})
    SET (typeDirFiles ${typeDirFiles} Type/${FILE})
ENDFOREACH()
# codegen directory
INCLUDE (CodeGen/CMakeLists.txt)
FOREACH (FILE ${FILES})
    SET (codeGenDirFiles ${codeGenDirFiles} CodeGen/${FILE})
ENDFOREACH()

# add executable files
ADD_EXECUTABLE(${TARGET_NAME} 
    ${grammarDirFiles} 
    ${lexerDirFiles}
    ${parserDirFiles}
    ${astDirFiles}
    ${typeDirFiles}
    ${runtimeDirFiles}
    ${codeGenDirFiles}
    main.cpp 
    )
# set binary director
SET (EXECUTABLE_BINARY_DIR Build)
SET (EXECUTABLE_OUTPUT_PATH
    ${PROJECT_BINARY_DIR}
    CACHE PATH
    "build and bin directory"
    )

# look for xml2 library
FIND_LIBRARY(XML2_LIBRARY
    NAMES xml2
    PATH /usr/lib /usr/local/lib
    )
FIND_PATH(XML2_INCLUDES parser.h
    /usr/local/include
    /usr/include
    )
INCLUDE_DIRECTORIES( ${XML2_INC_DIR}
    ${PROJECT_SOURCE_DIR}/Include
    )
IF(WIIN32)
    TARGET_LINK_LIBRARIES(${TARGET_NAME} ${XML2_LIBRARY}, mingw32)
ELSE(WIN32)
    TARGET_LINK_LIBRARIES(${TARGET_NAME} ${XML2_LIBRARY})
ENDIF(WIN32)

MESSAGE(STATUS "Generating Makefile for linux...")
SET (CMAKE_SYSTEM_NAME Linux) 
# set compiler, the g++ will be used both in linux or windows
SET (CMAKE_C_COMPILER gcc)
SET (CMAKE_CXX_COMPILER g++)
INCLUDE_DIRECTORIES(${PROJECT_SOURECE_DIR}/Include /usr/include/libxml2)
LINK_DIRECTORIES(/usr/lib/)
#set default iinstall prefix project root directory
IF (CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
    SET (CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}")
ENDIF()

#display status message for important variables
MESSAGE(STATUS "-------------------------------------------------")
MESSAGE(STATUS "PROJECT_NAME = ${PROJECT_NAME}")
MESSAGE(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
MESSAGE(STATUS "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
MESSAGE(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
MESSAGE(STATUS "------------------------------------------------")
MESSAGE(STATUS)
