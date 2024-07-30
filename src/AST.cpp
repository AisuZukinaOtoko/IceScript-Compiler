#include "AST.h"
#include "Lexical Analyser.h"
#include <iostream>
#include <iomanip>

static void addNode(const Node& parent, AST_NODE& parentAST) {
	if (parent.typeFlags == Node::IDENTIFIER) {
		parentAST.childNodes.emplace_back(AST_NODE::IDENTIFIER, parent.tokenIndex);
		return;
	}
	else if (parent.typeFlags >= Node::EXPRESSION && parent.typeFlags <= Node::FACTOR) {
		parentAST.childNodes.emplace_back(AST_NODE::VALUE, parent.tokenIndex);
		return;
	}
	else if (parent.typeFlags >= Node::BOOL_EXPR1 && parent.typeFlags <= Node::BOOL_EXPR5) {
		parentAST.childNodes.emplace_back(AST_NODE::VALUE, parent.tokenIndex);
		return;
	}

	switch (parent.typeFlags) {
	case Node::DECLARATION:
		parentAST.childNodes.emplace_back(AST_NODE::DECLARATION, parent.tokenIndex);
		break;
	case Node::CONSTANT:
		parentAST.childNodes.emplace_back(AST_NODE::CONSTANT, parent.tokenIndex);
		break;
	case Node::ASSIGNMENT:
		parentAST.childNodes.emplace_back(AST_NODE::ASSIGNMENT, parent.tokenIndex);
		break;
	case Node::BOOL_NOT_OPERATOR:
		parentAST.childNodes.emplace_back(AST_NODE::NOT, parent.tokenIndex);
		break;
	case Node::ROOT:
		parentAST.childNodes.emplace_back(AST_NODE::ROOT, parent.tokenIndex);
		break;
	case Node::WHILE:
		parentAST.childNodes.emplace_back(AST_NODE::WHILE, parent.tokenIndex);
		break;
	case Node::IF:
		parentAST.childNodes.emplace_back(AST_NODE::IF, parent.tokenIndex);
		break;
	case Node::ELSE:
		parentAST.childNodes.emplace_back(AST_NODE::ELSE, parent.tokenIndex);
		break;
	case Node::BREAK:
		parentAST.childNodes.emplace_back(AST_NODE::BREAK, parent.tokenIndex);
		break;

	case Node::TERMINAL:
	{
		std::string tokenValue = ipc_lexer.tokenValue(parent.tokenIndex);
		//std::cout << tokenValue << std::endl;
		if (tokenValue == "and")
			parentAST.childNodes.emplace_back(AST_NODE::AND, parent.tokenIndex);
		else if (tokenValue == "or")
			parentAST.childNodes.emplace_back(AST_NODE::OR, parent.tokenIndex);
		else if (tokenValue == "+")
			parentAST.childNodes.emplace_back(AST_NODE::ADD, parent.tokenIndex);
		else if (tokenValue == "-")
			parentAST.childNodes.emplace_back(AST_NODE::SUBTRACT, parent.tokenIndex);
		else if (tokenValue == "*")
			parentAST.childNodes.emplace_back(AST_NODE::MULTIPLY, parent.tokenIndex);
		else if (tokenValue == "/")
			parentAST.childNodes.emplace_back(AST_NODE::DIVIDE, parent.tokenIndex);
		else if (tokenValue == "=")
			parentAST.childNodes.emplace_back(AST_NODE::ASSIGN, parent.tokenIndex);
		break;
	}
	case Node::BOOL_OPERATOR:
	{
		std::string tokenValue = ipc_lexer.tokenValue(parent.tokenIndex);
		//std::cout << "Bool operator: " << tokenValue << std::endl;
		if (tokenValue == "<")
			parentAST.childNodes.emplace_back(AST_NODE::SMALLER, parent.tokenIndex);
		else if (tokenValue == ">")
			parentAST.childNodes.emplace_back(AST_NODE::LARGER, parent.tokenIndex);
		else if (tokenValue == "==")
			parentAST.childNodes.emplace_back(AST_NODE::EQUAL, parent.tokenIndex);
		break;
	}
	case Node::DATA_TYPE:
	{
		std::string tokenValue = ipc_lexer.tokenValue(parent.tokenIndex);
		if (tokenValue == "int")
			parentAST.childNodes.emplace_back(AST_NODE::INTEGER, parent.tokenIndex);
		else if (tokenValue == "float")
			parentAST.childNodes.emplace_back(AST_NODE::FLOAT, parent.tokenIndex);
		else if (tokenValue == "bool")
			parentAST.childNodes.emplace_back(AST_NODE::BOOLEAN, parent.tokenIndex);

		break;
	}
	default:
		parentAST.childNodes.emplace_back(AST_NODE::CONTINUE, parent.tokenIndex);
		break;
	}
}

void AST::printAST(AST_NODE& parent, int depth) {
	std::cout << std::left << std::setw(depth * 3) << "" << "["
		<< nodeTypes[parent.type] << ": " 
		<< parent.childNodes.size() << "]\n";
	for (AST_NODE& node : parent.childNodes) {
		printAST(node, depth + 1);
	}
}

void AST::createAST() {
	nodeTypes = {
		"Identifier", "Constant", "String", "True", "False",
		"If", "Else", "While", "Break", "Continue",
		"Add", "Subtract", "Divide", "Multiply",
		"And", "Or", "Not", "Value", "Assign",
		"Larger", "Smaller", "Equal",
		"Declaration", "Assignment", "Boolean", "Float", "Integer",
		"Root"
	};

	root.type = AST_NODE::ROOT;
	parseTree(ipc_parseTree.rootNode, root);
}

void AST::parseTree(const Node& parent, AST_NODE& parentAST) {
	if (!parent.childNodes.size()) {
		addNode(parent, parentAST);
		return;
	}

	if (parent.childNodes.size() == 1) {
		if (parent.typeFlags == Node::BOOL_NOT_OPERATOR) {
			addNode(parent, parentAST);
			parseTree(parent.childNodes[0], parentAST.childNodes.back());
			return;
		}
		else if (parent.typeFlags == Node::ROOT) {
			addNode(parent, parentAST);
			parseTree(parent.childNodes[0], parentAST.childNodes.back());
			return;
		}
		else {
			parseTree(parent.childNodes[0], parentAST);
			return;
		}
	}

	addNode(parent, parentAST);

	for (const Node& node : parent.childNodes) {
		parseTree(node, parentAST.childNodes.back());
	}
}