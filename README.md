TLS(Toyable Language Suit)
=======
	
1.Introduction
------
	tls is toyable language suit that include the following components.
	tlang
	tvm
	tlib
###tlang
	tlang is language core that implement the compiler, linker, assembler and debuger.
###tvm
	tvm is virtual machine using JIT as a runtime excution environment for tlang bytecode
###tlib
	tlib is library for tlang' user that provide helper class to access file system, opengl,etc.
2.Repo layout
-----
###tlang
	-compiler
	-linker
	-assembler
	-debuger
	-lib
###tvm
###tlib
###external
	for third party libs, such as Skia graphic lib
3.Design
-----
	1.language syntax like c++.
	2.all things are objects.
	3.do not use pointer, only object ref.
	4.source files are .th, .tpp file that is header file and source file.
	5.each source file have many classes.
	6.each class is compiled into a .tof that is tlang object file.
	7.many tof files are linked into a .tolib file that is tlang object library.
	8.tvm will load .tolib files to run the bytecode.
	9.the class is declared in .th file and implemented in .tpp file.
	10.use simple syntax that don't like c++.
	11.all things you like can be added into tls by you.
4.Goal
----
	the initial goal is to lean compiler principle, so the lexer and parser's generator
	will not be used. all things shoud be done by hand. I think it is interesting.
	as you know, only by doing like this, you can really understand the principle.
	we also don't use llvm although it is very good. although the methods are different,
	our goal is same.
	our goal is to provide an environment to understand the principle, and provide a new
	language for interesting.
5.Actions
----
	we will release verion 0.1 till the end of 2013 which will provide a basic unable evnironment.
	the version 0.2 will release in 2014.
6.enviroment dependcies
----
	gcc - all souces files are implemented by C++, so c++ compiler is needed, now using gcc
	stl	- c++ STL	
	cmake - make Makefile by CMakeLists.txt
	libxml - used by tlang compiler to output parse tre and AST into xml file
	
