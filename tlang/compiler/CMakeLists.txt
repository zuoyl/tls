cmake_minimum_required(VERSION 2.8)
PROJECT(TCC)
INCLUDE_DIRECTORIES(./Include /usr/include/TParser)
SET(SRC_LIST  
				main.cpp
				AST/AST.cpp 
				AST/ASTBuilder.cpp 
				AST/Class.cpp 
				AST/Method.cpp 
				AST/Struct.cpp 
				AST/Variable
				Lexer/Lexer.cpp
				Parser/Parser.cpp
				Builder/TypeBuilder.cpp
				Builder/IRBuilder.cpp
				Builder/IREmiter.cpp
				Builder/IROptimizer.cpp
				Runtime/Exception.cpp
				Runtime/Frame.cpp
				Runtime/Label.cpp
				Runtime/OSWrapper.cpp
				Runtime/Scope.cpp
				Runtime/Temp.cpp
                Runtime/Compile.cpp
                Runtime/Error.cpp
                Type/Type.cpp
				Type/BuiltinType.cpp
				Type/IntType.cpp
				Type/BoolType.cpp
				Type/FloatType.cpp
				Type/SetType.cpp
				Type/MapType.cpp
				Type/StringType.cpp
				Type/ClassType.cpp
                Type/MethodType.cpp
				)
MESSAGE(STATUS, "The binary dir" ${TCC_BINARY_DIR})
MESSAGE(STATUS, "The source dir" ${TCC_SOURCE_DIR})
ADD_EXECUTABLE(tcc ${SRC_LIST})
TARGET_LINK_LIBRARIES(tcc libTParser.a)

