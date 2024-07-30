#include "src/Lexical Analyser.h"
#include "src/Parse Tree.h"
#include "src/AST.h"
#include "src/Interpreter.h"
#include "src/Symbol Table.h"
#include <iostream>

int main(int argc, char* argv[]) {
	if (argc == 1) {
		std::cout << "No input file. Stop." << std::endl;
		return -1;
	}

	unsigned int flags = 0;

	for (int i = 2; i < argc; i++) {
		if (argv[i] == "-r")
			flags |= 0b1;
		if (argv[i] == "-d")
			flags |= 0b10;
	}

	ipc_lexer.tokenize(argv[1]);
	if (ipc_lexer.failed()) {
		return -1;
	}	

	ipc_parseTree.generateParseTree();
	if (ipc_parseTree.parseTreeFailed()) {
		return -1;
	}

	ipc_AST.createAST();
	if (flags & 0b10)
		ipc_AST.printAST(ipc_AST.root, 0);

	if (flags & 0b1) {
		ipc_interpreter.interpret(ipc_AST.root);
		ipc_symbolTable.printSymbols();
	}

	return 0;
}