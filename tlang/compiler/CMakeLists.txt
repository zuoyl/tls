cmake_minimum_required(VERSION 2.8)
PROJECT(TCC)
INCLUDE_DIRECTORIES(./Include )
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
                Parser/TParser.cpp
                Parser/FA.cpp
                Parser/TGrammar.cpp
                Parser/Tokens.cpp
                CodeGen/IRBuilder.cpp
                CodeGen/IREmiter.cpp
				Optimizer/IROptimizer.cpp
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
				Type/TypeBuilder.cpp
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
SET(CMAKE_CXX_FLAGS "-g")
ADD_EXECUTABLE(tcc ${SRC_LIST})

