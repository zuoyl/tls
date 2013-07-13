TLS(Toyable Language Suit)
----
1.Introduction
####
	tls is toyable language suit that include the following components.
	tlang - tlang is language core that implement the compiler, linker, assembler and debuger.
	tvm - tvm is virtual machine using JIT as a runtime excution environment for tlang bytecode
	tlib - tlib is library for tlang' user that provide helper class to access file system, opengl,etc.
2.Repo layout
####
    tlang
		-compiler
		-linker
		-assembler
		-debuger
		-lib
	tvm
	tlib
	external - for third party libs, such as Skia graphic lib
3.Design
####
	.language syntax like c++.
	.all things are objects.
	.do not use pointer, only object ref.
	.source files are .th, .tpp file that is header file and source file.
	.class declaration is only allowed in .th file
	.class implementation is only allowed in .tpp file
	.each source file have many classes.
	.each class is compiled into a .tof that is tlang object file.
	.many tof files are linked into a .tolib file that is tlang object library.
	.tvm will load .tolib files to run the bytecode.
	.the class is declared in .th file and implemented in .tpp file.
	.use simple syntax that don't like c++.
	.all things you like can be added into tls by you.
4.Goal
####
	the initial goal is to lean compiler principle, so the lexer and parser's generator
	will not be used. all things shoud be done by hand. I think it is interesting.
	as you know, only by doing like this, you can really understand the principle.
	we also don't use llvm although it is very good. although the methods are different,
	our goal is same.
	our goal is to provide an environment to understand the principle, and provide a new
	language for interesting.
5.Actions
####
	we will release verion 0.1 till the end of 2013 which will provide a basic runable evnironment.
	the version 0.2 will release in 2014.
6.Enviroment dependcy
####
    Mac os, cgywin with libxml2, cmake are supported. 
	gcc - all souces files are implemented by C++, so c++ compiler is needed, now using gcc
	stl	- c++ STL	
	cmake - make Makefile by CMakeLists.txt
	libxml - used by tlang compiler to output parse tre and AST into xml file
	
