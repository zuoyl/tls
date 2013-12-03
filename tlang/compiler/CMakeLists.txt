#cmake file for buiding src/tlang-compiler package
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
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src")

# c++ definitions
ADD_DEFINITIONS("-Wall")
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS("-DDEBUG")
ENDIF()

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
# add executable files
ADD_EXECUTABLE(${TARGET_NAME} 
    src/tl-token.cpp
    src/tl-fa.cpp       
    src/tl-grammar.cpp      
    src/tl-lexer.cpp        
    src/tl-parser.cpp       
    src/tl-ast.cpp      
    src/tl-ast-expr.cpp     
    src/tl-ast-class.cpp        
    src/tl-ast-method.cpp       
    src/tl-ast-variable.cpp
    src/tl-ast-builder.cpp 
    src/tl-ast-xml.cpp      
    src/tl-scope.cpp        
    src/tl-type.cpp
    src/tl-type-builder.cpp 
    src/tl-type-set.cpp
    src/tl-type-int.cpp     
    src/tl-type-float.cpp   
    src/tl-type-class.cpp   
    src/tl-type-string.cpp
    src/tl-type-bool.cpp    
    src/tl-type-map.cpp     
    src/tl-exception.cpp    
    src/tl-ir-emiter.cpp    
    src/tl-ir-builder.cpp   
    src/tl-ir-optimizer.cpp 
    src/tl-error.cpp        
    src/tl-temp.cpp     
    src/tl-value.cpp
    src/tl-os.cpp       
    src/tl-frame.cpp        
    src/tl-lable.cpp        
    src/tl-compile.cpp      
    src/tl-main.cpp     
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
    ${PROJECT_SOURCE_DIR}/src
    )
IF(WIN32)
    TARGET_LINK_LIBRARIES(${TARGET_NAME} ${XML2_LIBRARY}, src mingw32)
ELSE(WIN32)
    TARGET_LINK_LIBRARIES(${TARGET_NAME} ${XML2_LIBRARY})
ENDIF(WIN32)

MESSAGE(STATUS "Generating Makefile for linux...")
SET (CMAKE_SYSTEM_NAME Linux) 
# set compiler, the g++ will be used both in linux or windows
SET (CMAKE_C_COMPILER gcc)
SET (CMAKE_CXX_COMPILER g++)
INCLUDE_DIRECTORIES(${PROJECT_SOURECE_DIR}/src /usr/include/libxml2)
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
