#pragma once
#include "AST.h"

class Interpreter {
public:
	void interpret(const AST_NODE& root);
private:
	bool failed = false;
	void rootNode(const AST_NODE& root);
	void declNode(const AST_NODE& root);
	void assgNode(const AST_NODE& root);
	void whileNode(const AST_NODE& root);
	void ifNode(const AST_NODE& root);
	double valueNode(const AST_NODE& root);
	double terminalValueNode(const AST_NODE& root);

	void logError(std::string error, int tokenIndex);
	void logWarning(std::string error, int tokenIndex);

	bool breakLoop = false;
};

inline Interpreter ipc_interpreter;