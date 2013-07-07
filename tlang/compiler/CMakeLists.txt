cmake_minimum_required(VERSION 2.8)
IF(WIN32)
    MESSAGE(STATUS "Generating Makefile for windows...") 
    SET(CMAKE_SYSTEM_NAME Windows)
    SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    SET(CMAKE_FIND_PATH_MODE_LIBRARY ONLY)
    SET(CMAKE_FIND_PATH_MODE_INCLUDE ONLY)
ELSE(WIN32)
    MESSAGE(STATUS "Generating Makefile for linux...")
ENDIF(WIN32)
PROJECT(TCC)

INCLUDE_DIRECTORIES(./Include /usr/include/libxml2)
SET(CMAKE_BUILD_TYPE DEBUG)
SET(SRC_LIST  
				main.cpp
                Grammar/Grammar.cpp
                Lexer/Lexer.cpp
				Parser/Parser.cpp
                Parser/Tokens.cpp
                Parser/FA.cpp
				Runtime/Exception.cpp
				Runtime/Frame.cpp
				Runtime/Label.cpp
                Runtime/OSWrapper.cpp
				Runtime/Scope.cpp
				Runtime/Temp.cpp
                Runtime/Compile.cpp
                Runtime/Error.cpp
				AST/AST.cpp 
				AST/ASTBuilder.cpp 
				AST/Class.cpp 
				AST/Method.cpp 
                AST/Expression.cpp
                AST/Variable
                AST/ASTxml.cpp
				)
MESSAGE(STATUS, "The binary dir" ${TCC_BINARY_DIR})
MESSAGE(STATUS, "The source dir" ${TCC_SOURCE_DIR})
ADD_EXECUTABLE(tcc ${SRC_LIST})
LINK_DIRECTORIES(/usr/lib/)
TARGET_LINK_LIBRARIES(tcc xml2)
