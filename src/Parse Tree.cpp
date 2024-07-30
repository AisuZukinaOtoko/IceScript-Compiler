#include "Parse Tree.h"
#include "Lexical Analyser.h"
#include "Symbol Table.h"
#include "Source manager.h"
#include <iostream>
#include <iomanip>
#include <Windows.h>

void ParseTree::addNode(Node& parentNode, int typeFlags) {
	Node tempNode;
	tempNode.typeFlags = typeFlags;
	tempNode.tokenIndex = currentTokenIndex;
	parentNode.childNodes.push_back(tempNode);
}

void ParseTree::printTree(Node & root, int depth) {
	std::cout << std::left << std::setw(depth * 3) << "";
	switch (root.typeFlags) {
	case Node::IDENTIFIER:
		std::cout << "Identifier: ";
		break;
	case Node::TERMINAL:
		std::cout << "Terminal: ";
		break;
	case Node::BOOL_EXPR1:
		std::cout << "bEx1: ";
		break;
	case Node::BOOL_EXPR2:
		std::cout << "bEx2: ";
		break;
	case Node::BOOL_EXPR3:
		std::cout << "bEx3: ";
		break;
	case Node::BOOL_EXPR4:
		std::cout << "bEx4: ";
		break;
	case Node::BOOL_NOT_OPERATOR:
		std::cout << "bNOT: ";
		break;
	case Node::BOOL_OPERATOR:
		std::cout << "Bool operator: ";
		break;
	case Node::EXPRESSION:
		std::cout << "AEx1: ";
		break;
	case Node::TERM:
		std::cout << "Term: ";
		break;
	case Node::FACTOR:
		std::cout << "Factor: ";
		break;
	case Node::SEMI_COLON:
		std::cout << "Semi colon: ";
		break;
	case Node::WHILE:
		std::cout << "While: ";
		break;
	case Node::ROOT:
		std::cout << "Root: ";
		break;
	default:
		std::cout << "Something else: ";
		break;
	}
	std::cout << root.childNodes.size() << std::endl;
	for (Node& node : root.childNodes) {
		printTree(node, depth + 1);
	}
}

void ParseTree::errorMissingSymbol(std::string error, int tokenIndex) {
	if (parserFailed)
		return;
	std::cout << "Line " + std::to_string(ipc_lexer.tokens[currentTokenIndex].tokenLineNumber) + ": " << error << std::endl;
	ipc_sourceManager.errMissingSymbol(ipc_lexer.tokens[tokenIndex]);
	parserFailed = true;
}

void ParseTree::errorSymbol(std::string error, int tokenIndex) {
	if (parserFailed)
		return;
	std::cout << "Line " + std::to_string(ipc_lexer.tokens[currentTokenIndex].tokenLineNumber) + ": " << error << std::endl;
	ipc_sourceManager.showError(ipc_lexer.tokens[tokenIndex]);
	parserFailed = true;
}

bool ParseTree::parseTreeFailed() {
	return parserFailed;
}

void ParseTree::generateParseTree() {
	currentTokenIndex = 0;
	rootNode.typeFlags = Node::ROOT;
	parseRoot(rootNode);
}

void ParseTree::parseRoot(Node& root) {
	int currentTokenType = ipc_lexer.tokens[currentTokenIndex].tokenType;

	while (currentTokenType != Token::UNKNOWN && !parserFailed) {
		Token& currentToken = ipc_lexer.tokens[currentTokenIndex];

		if (currentTokenType & Token::PUNCTUATOR) {
			if (ipc_lexer.tokenValue(currentTokenIndex) == "}") {
				return;
				break;
			}
			else {
				errorSymbol("Unexpected token in root.", currentTokenIndex);
				currentTokenIndex--;
				return;
			}
		}
		else if (currentTokenType == Token::KEYWORD) {
			parseKeyword(root);
		}
		else if (currentTokenType == Token::IDENTIFIER) {
			if (ipc_lexer.tokens[currentTokenIndex + 1].tokenType & Token::ASSIGNMENT) { // an assignment expression
				addNode(root, Node::ASSIGNMENT);
				parseAssignment(root.childNodes.back());
			}
			else if (ipc_lexer.tokens[currentTokenIndex + 1].tokenType & Token::OPEN_PARENTHESIS) { // a function call
				std::cout << "Function call\n";
			}
			else {
				errorSymbol("Unexpected token in root.", currentTokenIndex);
				currentTokenIndex--;
				return;
			}
		}
		else {
			errorSymbol("Unexpected token in root.", currentTokenIndex);
			currentTokenIndex--;
		}
		currentTokenIndex++;
		currentTokenType = ipc_lexer.tokens[currentTokenIndex].tokenType;
	}
}

void ParseTree::parseKeyword(Node& parentNode) {
	const int dataTypeCount = 3;
	std::string dataTypes[dataTypeCount] = {"int", "float", "bool"};

	Node tempNode;
	// A variable declaration
	for (int i = 0; i < dataTypeCount; i++) {
		if (ipc_lexer.tokenValue(currentTokenIndex) == dataTypes[i]) {
			addNode(parentNode, Node::DECLARATION);
			parseDeclaration(parentNode.childNodes.back());

			if (ipc_lexer.tokens[currentTokenIndex].tokenType & Token::IDENTIFIER) {
				addNode(parentNode.childNodes.back(), Node::IDENTIFIER);

				if (ipc_lexer.tokenValue(currentTokenIndex + 1) == "=") {
					addNode(parentNode, Node::ASSIGNMENT);
					parseAssignment(parentNode.childNodes.back());
				}
				else if (ipc_lexer.tokenValue(currentTokenIndex + 1) == ";") {
					currentTokenIndex++;
				}
				else { // Error
					errorMissingSymbol("Expected an ';'.", currentTokenIndex);
				}
			}
			else { // Error
				errorSymbol("Expected an identifier", currentTokenIndex);
			}
			return;
		}
	}
	
	Token& currentToken = ipc_lexer.tokens[currentTokenIndex];
	// first check if keyword is a data type
	
	if (ipc_lexer.tokenValues[currentToken.tokenValueIndex] == "if") {
		tempNode.typeFlags = Node::IF;
		tempNode.tokenIndex = currentTokenIndex;
		parentNode.childNodes.push_back(tempNode);
		currentTokenIndex++;
		parseIf(parentNode.childNodes.back());
	}
	else if (ipc_lexer.tokenValues[currentToken.tokenValueIndex] == "while") {
		tempNode.typeFlags = Node::WHILE;
		tempNode.tokenIndex = currentTokenIndex;
		parentNode.childNodes.push_back(tempNode);
		currentTokenIndex++;
		loopDepth++;
		parseWhile (parentNode.childNodes.back());
		loopDepth--;
	}
	else if (ipc_lexer.tokenValues[currentToken.tokenValueIndex] == "break") {
		if (loopDepth > 0)
			addNode(parentNode, Node::BREAK);
		else
			errorSymbol("Break used outside of loop.", currentTokenIndex);
	}
	else if (ipc_lexer.tokenValue(currentTokenIndex) == "continue") {
		errorSymbol("Continue Keyword Not Implemented Yet.", currentTokenIndex);
		//addNode(parentNode, Node::CONTINUE);
	}
	else {
		errorSymbol("Invalid use of keyword.", currentTokenIndex);
		currentTokenIndex--;
	}
}

void ParseTree::parseDeclaration(Node& parentNode) {
	addNode(parentNode, Node::DATA_TYPE);
	currentTokenIndex++;
}

void ParseTree::parseExpression(Node& parentNode) {

	int tokenType = ipc_lexer.tokens[currentTokenIndex].tokenType;
	if (tokenType == Token::UNKNOWN || tokenType == Token::END_STATEMENT) { // Error
		errorMissingSymbol("Expected an expression.", currentTokenIndex - 1);
		currentTokenIndex--;
		return;
	}

	Node tempLeftNode;
	Node tempMiddleNode;
	if (parentNode.typeFlags & Node::EXPRESSION) { // Handles addition and subtraction expressions
		// Left term node
		tempLeftNode.typeFlags = Node::TERM;
		parentNode.childNodes.push_back(tempLeftNode);
		parseExpression(parentNode.childNodes.back());
		currentTokenIndex++;

		if (ipc_lexer.tokens[currentTokenIndex].tokenType & Token::END_STATEMENT) {
			currentTokenIndex--;
			return;
		}
		if (ipc_lexer.tokens[currentTokenIndex].tokenType & (Token::BOOLEAN_AND | Token::BOOLEAN_NOT | Token::BOOLEAN_OR | Token::COMPARISON)) {
			currentTokenIndex--;
			return;
		}

		// Middle node
		if (ipc_lexer.tokenValue(currentTokenIndex) == "+" || ipc_lexer.tokenValue(currentTokenIndex) == "-") {
			addNode(parentNode, Node::TERMINAL);
			currentTokenIndex++;
			parseExpression(parentNode);
		}
		else if (openParenthesisCount > 0 && ipc_lexer.tokenValue(currentTokenIndex) == ")") {
			currentTokenIndex--;
			return;
		}
		else { // Error
			currentTokenIndex--;
			return;
			parserFailed = true;
			if (openParenthesisCount > 0)
				return;

			errorMissingSymbol("Expression Expected an ';'.", currentTokenIndex - 1);
			currentTokenIndex--;
			return;
		}
	}
	else if (parentNode.typeFlags & Node::TERM) { // Handles multiplication and subtraction terms
		// Left factor node
		tempLeftNode.typeFlags = Node::FACTOR;
		parentNode.childNodes.push_back(tempLeftNode);
		parseExpression(parentNode.childNodes.back());
		currentTokenIndex++;

		// Middle node
		if (ipc_lexer.tokenValue(currentTokenIndex) == "*" || ipc_lexer.tokenValue(currentTokenIndex) == "/") {
			addNode(parentNode, Node::TERMINAL);
			currentTokenIndex++;
			parseExpression(parentNode);
		}
		else {
			currentTokenIndex--;
			return;
		}
	}
	else if (parentNode.typeFlags & Node::FACTOR) { // Handles factors
		if (tokenType & Token::OPEN_PARENTHESIS) {
			openParenthesisCount++;
			currentTokenIndex++;
			addNode(parentNode, Node::BOOL_EXPR1);
			parseBoolExpr(parentNode.childNodes.back());

			// after expression is parsed, check for matching brackets
			tokenType = ipc_lexer.tokens[currentTokenIndex].tokenType;
			if (tokenType & Token::CLOSE_PARENTHESIS) {
				openParenthesisCount--;
				return;
			}
			// Error
			errorMissingSymbol("Expression Expected parenthesis.", currentTokenIndex - 1);
			currentTokenIndex--;
			return;
		}

		switch (tokenType) {
		case Token::IDENTIFIER:
			tempLeftNode.typeFlags = Node::IDENTIFIER;
			break;
		case Token::CONSTANT:
			tempLeftNode.typeFlags = Node::CONSTANT;
			break;
		case Token::KEYWORD | Token::CONSTANT:
			tempLeftNode.typeFlags = Node::IDENTIFIER;
			break;
		case Token::LITERAL:
			tempLeftNode.typeFlags = Node::TERMINAL | Node::STRING_LITERAL;
			break;
		default: // Error
			errorMissingSymbol("Expected an expression.", currentTokenIndex - 1);
			currentTokenIndex--;
			return;
		}
		tempLeftNode.tokenIndex = currentTokenIndex;
		parentNode.childNodes.push_back(tempLeftNode);
	}
	else { // Error
		errorSymbol("Expected an expression.", currentTokenIndex - 1);
		currentTokenIndex--;
	}
}

void ParseTree::parseBoolExpr(Node& parentNode) {

	int tokenType = ipc_lexer.tokens[currentTokenIndex].tokenType;
	if (tokenType == Token::UNKNOWN || tokenType == Token::END_STATEMENT) { // Error
		errorMissingSymbol("Expected a boolean expression.", currentTokenIndex - 1);
		currentTokenIndex--;
		return;
	}

	Node tempLeftNode;
	Node tempMiddleNode;

	if (parentNode.typeFlags == Node::BOOL_EXPR1) {
		// Left term node
		tempLeftNode.typeFlags = Node::BOOL_EXPR2;
		parentNode.childNodes.push_back(tempLeftNode);
		parseBoolExpr(parentNode.childNodes.back());
		currentTokenIndex++;

		if (ipc_lexer.tokens[currentTokenIndex].tokenType & Token::END_STATEMENT) {
			return;
		}

		// Middle node
		if (ipc_lexer.tokens[currentTokenIndex].tokenType & Token::BOOLEAN_OR) {
			addNode(parentNode, Node::TERMINAL);
			currentTokenIndex++;
			parseBoolExpr(parentNode);
		}
		else if (openParenthesisCount > 0 && ipc_lexer.tokenValue(currentTokenIndex) == ")") {
			return;
		}
		else { // Error
			return;
			parserFailed = true;
			errorMissingSymbol("Bool Expected an ';'.", currentTokenIndex - 1);
			currentTokenIndex--;
			return;
		}
	}
	else if (parentNode.typeFlags == Node::BOOL_EXPR2) { // DONE!!!
		// Left factor node
		tempLeftNode.typeFlags = Node::BOOL_EXPR3;
		parentNode.childNodes.push_back(tempLeftNode);
		parseBoolExpr(parentNode.childNodes.back());
		currentTokenIndex++;

		// Middle node
		if (ipc_lexer.tokens[currentTokenIndex].tokenType & Token::BOOLEAN_AND) {
			addNode(parentNode, Node::TERMINAL);
			currentTokenIndex++;
			parseBoolExpr(parentNode);
		}
		else {
			currentTokenIndex--;
			return;
		}
	}
	else if (parentNode.typeFlags == Node::BOOL_EXPR3) { // DONE!!!
		addNode(parentNode, Node::BOOL_EXPR4);
		parseBoolExpr(parentNode.childNodes.back());
		currentTokenIndex++;

		if (ipc_lexer.tokens[currentTokenIndex].tokenType & Token::COMPARISON) {
			addNode(parentNode, Node::BOOL_OPERATOR);
			currentTokenIndex++;

			addNode(parentNode, Node::BOOL_EXPR4);
			parseBoolExpr(parentNode.childNodes.back());
		}
		else {
			currentTokenIndex--;
		}
	}
	else if (parentNode.typeFlags == Node::BOOL_EXPR4) { // DONE!!!
		if (tokenType & Token::BOOLEAN_NOT) {
			addNode(parentNode, Node::BOOL_NOT_OPERATOR);
			currentTokenIndex++;
			parseBoolExpr(parentNode.childNodes.back());
			return;
		}

		addNode(parentNode, Node::BOOL_EXPR5);
		parseBoolExpr(parentNode.childNodes.back());

	}
	else if (parentNode.typeFlags == Node::BOOL_EXPR5) {
		addNode(parentNode, Node::EXPRESSION);
		parseExpression(parentNode.childNodes.back());
	}
	else if (parentNode.typeFlags == Node::BOOL_NOT_OPERATOR) { // DONE!!!		
		addNode(parentNode, Node::FACTOR);
		parseExpression(parentNode.childNodes.back());
	}
	else { // Error
		errorSymbol("Bool Expected an expression.", currentTokenIndex - 1);
		currentTokenIndex--;
	}
}

void ParseTree::parseAssignment(Node& parentNode) {
	Node identifierNode;
	Node operatorNode;
	Node expressionNode;
	Node endStatementNode;

	// identifier
	addNode(parentNode, Node::IDENTIFIER);
	currentTokenIndex++;

	// assignment operator
	addNode(parentNode, Node::TERMINAL);
	currentTokenIndex++;

	// parse expression
	expressionNode.typeFlags = Node::BOOL_EXPR1;
	parentNode.childNodes.push_back(expressionNode);
	parseBoolExpr(parentNode.childNodes.back());

	if (parserFailed)
		return;

	// end statement
	if (ipc_lexer.tokens[currentTokenIndex].tokenType & Token::END_STATEMENT) {
		//addNode(parentNode, Node::SEMI_COLON);
		return;
	}
	//else if (ipc_lexer.tokens[currentTokenIndex].tokenType == Token::UNKNOWN) { // Error
	//	std::cout << "Missing ';'" << std::endl;
	//	currentTokenIndex--;
	//	parserFailed = true;
	//}
	else { // Error
		errorMissingSymbol("Assignment Expected ';'.", currentTokenIndex - 1);
		currentTokenIndex--;
		parserFailed = true;
	}
}

void ParseTree::parseIf(Node& parentNode) {
	Node tempNode;

	if (ipc_lexer.tokenValue(currentTokenIndex) == "(") {
		addNode(parentNode, Node::BOOL_EXPR1);
		parseBoolExpr(parentNode.childNodes.back());
	}
	else {
		errorMissingSymbol("IF Expected '('.", currentTokenIndex - 1);
		return;
	}

	if (parserFailed)
		return;

	if (ipc_lexer.tokenValue(currentTokenIndex) == "{") {
		addNode(parentNode, Node::ROOT);
		currentTokenIndex++;
		parseRoot(parentNode.childNodes.back());


		if (ipc_lexer.tokenValue(currentTokenIndex) == "}") {
			currentTokenIndex++;
		}
		else {
			currentTokenIndex--;
			errorMissingSymbol("IF Expected '}'.", currentTokenIndex - 1);
			return;
		}

		if (ipc_lexer.tokenValue(currentTokenIndex) == "else") {
			currentTokenIndex++;
			if (ipc_lexer.tokenValue(currentTokenIndex) != "{") {
				currentTokenIndex--;
				errorMissingSymbol("Else Expected '{'.", currentTokenIndex - 1);
				return;
			}
			currentTokenIndex++;

			addNode(parentNode, Node::ROOT);
			parseRoot(parentNode.childNodes.back());

			if (ipc_lexer.tokenValue(currentTokenIndex) != "}") {
				currentTokenIndex--;
				errorMissingSymbol("Else Expected '}'.", currentTokenIndex - 1);
				return;
			}
		}
		else
			currentTokenIndex--;
		
	}
	else {
		errorMissingSymbol("IF Expected '{'.", currentTokenIndex - 1);
		currentTokenIndex--;
		return;
	}
}

void ParseTree::parseWhile(Node& parentNode) {
	Node tempNode;

	if (ipc_lexer.tokenValue(currentTokenIndex) == "(") {
		addNode(parentNode, Node::BOOL_EXPR1);
		parseBoolExpr(parentNode.childNodes.back());
	}
	else {
		errorMissingSymbol("While Expected '('.", currentTokenIndex - 1);
		return;
	}

	if (parserFailed)
		return;

	if (ipc_lexer.tokenValue(currentTokenIndex) == "{") {
		addNode(parentNode, Node::ROOT);
		currentTokenIndex++;
		parseRoot(parentNode.childNodes.back());


		if (ipc_lexer.tokenValue(currentTokenIndex) != "}") {
			currentTokenIndex--;
			errorMissingSymbol("While Expected '}'.", currentTokenIndex - 1);
			return;
		}

	}
	else {
		errorMissingSymbol("While Expected '{'.", currentTokenIndex - 1);
		currentTokenIndex--;
		return;
	}
}
