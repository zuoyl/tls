1.Build method
#####
	to build the compiler under your system, please run cmake at first in current folder.
	cmake .
	make
2.Debug method
####
	to debug the compiler, the DFA, parsetree and abstract syntax tree is dumped into xml file.
    ./tcc Test/robot.tpp
	for example, by the test source file in Test folder, robot.th, robot.tpp
	the grammar.dfa will be found in compiler folder, grammar_dfa.xml
	the robot.tpp.parse.xml and robot.tpp.ast.xml will be found in Test folder.
