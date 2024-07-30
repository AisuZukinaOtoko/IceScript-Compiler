#include "Interpreter.h"
#include "Lexical Analyser.h"
#include "Symbol Table.h"
#include "Source manager.h"
#include <iostream>

// Stops execution of the interpreter
void Interpreter::logError(std::string error, int tokenIndex) {
	failed = true;
	std::cout << "Line " + std::to_string(ipc_lexer.tokens[tokenIndex].tokenLineNumber) + ": " << error << std::endl;
	ipc_sourceManager.showError(ipc_lexer.tokens[tokenIndex]);
}

void Interpreter::logWarning(std::string warning, int tokenIndex) {
	std::cout << "Line " + std::to_string(ipc_lexer.tokens[tokenIndex].tokenLineNumber) + ": " << warning << std::endl;
	ipc_sourceManager.showError(ipc_lexer.tokens[tokenIndex]);
}

void Interpreter::interpret(const AST_NODE& parent) {
	rootNode(parent);
}

void Interpreter::rootNode(const AST_NODE& parent) {
	for (const AST_NODE& node : parent.childNodes) {
		if (failed || breakLoop)
			return;

		switch (node.type) {
		case AST_NODE::ROOT:
			rootNode(node);
			break;
		case AST_NODE::DECLARATION:
			declNode(node);
			break;
		case AST_NODE::ASSIGNMENT:
			assgNode(node);
			break;
		case AST_NODE::WHILE:
			whileNode(node);
			break;
		case AST_NODE::IF:
			ifNode(node);
			break;
		case AST_NODE::BREAK:
			breakLoop = true;
			return;
		}
	}
}

void Interpreter::declNode(const AST_NODE& parent) {
	std::string id = ipc_lexer.tokenValue(parent.childNodes[1].tokenIndex);
	if (ipc_symbolTable.addSymbol(id)) {
		return;
	}
	else {
		logError("Multiple declaration of variable.", parent.childNodes[1].tokenIndex);
	}
}

void Interpreter::assgNode(const AST_NODE& parent) {
	std::string id = ipc_lexer.tokenValue(parent.tokenIndex);
	Symbol* symbol = ipc_symbolTable.getSymbol(id);

	if (!symbol) {
		logError("Use of undefined variable.", parent.tokenIndex);
		return;
	}

	symbol->value = valueNode(parent.childNodes[2]);
	symbol->initWarning = false;
}

void Interpreter::whileNode(const AST_NODE& parent) {
	bool condition = valueNode(parent.childNodes[0]);
	while (condition && !breakLoop && !failed) {
		rootNode(parent.childNodes[1]);
		condition = valueNode(parent.childNodes[0]);
	}
	breakLoop = false;
}

void Interpreter::ifNode(const AST_NODE& parent) {
	bool condition = valueNode(parent.childNodes[0]);
	if (condition) {
		rootNode(parent.childNodes[1]);
		return;
	}
	
	if (parent.childNodes.size() == 3) {
		rootNode(parent.childNodes[2]);
	}

}

double Interpreter::valueNode(const AST_NODE& parent) {
	if (parent.type == AST_NODE::NOT) {
		return !valueNode(parent.childNodes[0]);
	}

	if (!parent.childNodes.size()) {
		return terminalValueNode(parent);
	}
	
	switch (parent.childNodes[1].type) {
	case AST_NODE::ADD:
		return valueNode(parent.childNodes[0]) + valueNode(parent.childNodes[2]);
	case AST_NODE::SUBTRACT:
		return valueNode(parent.childNodes[0]) - valueNode(parent.childNodes[2]);
	case AST_NODE::MULTIPLY:
		return valueNode(parent.childNodes[0]) * valueNode(parent.childNodes[2]);
	case AST_NODE::DIVIDE:
		return valueNode(parent.childNodes[0]) / valueNode(parent.childNodes[2]);
	case AST_NODE::EQUAL:
		return valueNode(parent.childNodes[0]) == valueNode(parent.childNodes[2]);
	case AST_NODE::LARGER:
		return valueNode(parent.childNodes[0]) > valueNode(parent.childNodes[2]);
	case AST_NODE::SMALLER:
		return valueNode(parent.childNodes[0]) < valueNode(parent.childNodes[2]);
	case AST_NODE::OR:
		return valueNode(parent.childNodes[0]) || valueNode(parent.childNodes[2]);
	case AST_NODE::AND:
		return valueNode(parent.childNodes[0]) && valueNode(parent.childNodes[2]);
	}
	return 0;
}

double Interpreter::terminalValueNode(const AST_NODE& parent) {
	switch (parent.type) {
	case AST_NODE::IDENTIFIER:
	{
		std::string id = ipc_lexer.tokenValue(parent.tokenIndex);
		Symbol* symbol = ipc_symbolTable.getSymbol(id);

		if (!symbol) {
			logError("Use of undefined variable.", parent.tokenIndex);
			return 0;
		}

		if (symbol->initWarning) {
			logWarning("Use of uninitialised variable", parent.tokenIndex);
		}

		return symbol->value;
	}
	case AST_NODE::CONSTANT:
	{
		std::string id = ipc_lexer.tokenValue(parent.tokenIndex);
		return std::stod(id);
	}
	}
	return 0;
}