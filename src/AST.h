#pragma once
#include "Parse Tree.h"

struct AST_NODE {
	enum {IDENTIFIER = 0, CONSTANT, STRING, TRUE, FALSE,
	IF, ELSE, WHILE, BREAK, CONTINUE,
	ADD, SUBTRACT, DIVIDE, MULTIPLY,
	AND, OR, NOT, VALUE, ASSIGN,
	LARGER, SMALLER, EQUAL,
	DECLARATION, ASSIGNMENT, BOOLEAN, FLOAT, INTEGER,
	ROOT,
	TYPE_COUNT};
	int type = 0;
	int tokenIndex = 0;
	std::vector<AST_NODE> childNodes;
	AST_NODE() {};
	AST_NODE(const Node& node) {
		type = node.typeFlags;
		tokenIndex = node.tokenIndex;
	}
	AST_NODE(int type, int tokenIndex) {
		this->type = type;
		this->tokenIndex = tokenIndex;
	}
};

class AST {
public:
	AST_NODE root;
	void createAST();
	void printAST(AST_NODE& parent, int depth);
private:
	std::vector<std::string> nodeTypes;
	void parseTree(const Node& parent, AST_NODE& parentAST);
};
inline AST ipc_AST;