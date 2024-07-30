#pragma once
#include <vector>
#include <string>

struct Node { 
	enum TYPE {ROOT, TERMINAL = 0x1, IDENTIFIER = 0x2, CONSTANT, STRING_LITERAL = 0x10, SEMI_COLON = 0x20, TRUE = 0x40, FALSE = 0x80,
		NON_TERMINAL = 0x100, IF, ELSE, WHILE, BREAK, CONTINUE, ASSIGNMENT = 0x200, EXPRESSION = 0x400, TERM = 0x800, FACTOR = 0x1000,
		BOOL_NODE = 0x2000, BOOL_EXPR1, BOOL_EXPR2,
		BOOL_EXPR3, BOOL_EXPR4, BOOL_EXPR5, BOOL_OPERATOR = 0x10000, BOOL_NOT_OPERATOR, DECLARATION = 0x20000, BOOLEAN = 0x40000, DATA_TYPE};
	int typeFlags = 0;
	int tokenIndex = -1;
	std::vector<Node> childNodes;	
};

class ParseTree {
public:
	void generateParseTree();
	bool parseTreeFailed();
	void printTree(Node& root, int depth);
	Node rootNode;
private:
	int currentTokenIndex = -1;
	int openParenthesisCount = 0;
	int loopDepth = 0;
	bool parserFailed = false;

	void addNode(Node& parentNode, int typeFlags);
	void errorSymbol(std::string err, int tokenIndex);
	void errorMissingSymbol(std::string err, int tokenIndex);

	void parseRoot(Node& root);
	void parseAssignment(Node& parentNode);
	void parseExpression(Node& parentNode);
	void parseBoolExpr(Node& parentNode);
	void parseDeclaration(Node& parentNode);
	void parseKeyword(Node& parentNode);
	void parseIf(Node& parentNode);
	void parseWhile(Node& parentNode);
};
inline ParseTree ipc_parseTree;