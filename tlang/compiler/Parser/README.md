###Note:
	The good method is like bison or flex to generate the .h,.cpp files
	which is included by compiler. however, there is no enough time for me to
	implement like that.
	the current method is following.
	1.edit the grammar.txt to add/modify/delete grammar
	2.use the TParser/TGrammar class to generate the status tansfer table
	3.the Lexer/Parser class use the table to parse
