cmake_minimum_required(VERSION 2.8)
PROJECT(TCC)
INCLUDE_DIRECTORIES(./Include /usr/include/libxml2)
SET(CMAKE_BUILD_TYPE DEBUG)
SET(SRC_LIST  
				main.cpp
				AST/AST.cpp 
				AST/ASTBuilder.cpp 
				AST/Class.cpp 
				AST/Method.cpp 
				AST/Variable
                AST/ASTxml.cpp
                Lexer/Lexer.cpp
				Parser/Parser.cpp
                Parser/GrammarParser.cpp
                Parser/FA.cpp
                Parser/Grammar.cpp
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
			    Type/Preproc.cpp	
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
LINK_DIRECTORIES(/usr/lib/)
TARGET_LINK_LIBRARIES(tcc xml2)
