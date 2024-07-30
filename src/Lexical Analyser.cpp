#include "Lexical Analyser.h"
#include "Source manager.h"
#include <fstream>
#include <iostream>
#include <Windows.h>

LexicalAnalyser::LexicalAnalyser() {
	lexicalAnalysisFailed = false;

	keyWords.emplace_back("if");
	keyWords.emplace_back("else");
	keyWords.emplace_back("true");
	keyWords.emplace_back("false");
	keyWords.emplace_back("for");
	keyWords.emplace_back("while");
	keyWords.emplace_back("break");
	keyWords.emplace_back("continue");
	keyWords.emplace_back("return");
	keyWords.emplace_back("int");
	keyWords.emplace_back("float");
	keyWords.emplace_back("bool");
	keyWords.emplace_back("char");
	keyWords.emplace_back("or");
	keyWords.emplace_back("and");
	keyWords.emplace_back("not");
}

void LexicalAnalyser::tokenize(const char* path) {
	std::ifstream inFile;
	inFile.open(path);

	if (inFile.fail()) {
		std::cerr << "Error: Could not open file " << path << std::endl;
		lexicalAnalysisFailed = true;
		return;
	}

	while (!inFile.eof()) {
		std::getline(inFile, tempLine);
		tempLine += ' '; // add an ending character to each line
		ipc_sourceManager.addLine(tempLine);
		TokenizeLine();
		lineNumber++;
		currentLineIndex = 0;
	}

	if (stringLiteral != '\0') { // A string has not been closed off
		ipc_sourceManager.showError("String on line " + std::to_string(stringBeginLine) + " exceeds end of file.");
		lexicalAnalysisFailed = true;
		return;
	}

	inFile.close();
	tokens.emplace_back(Token(Token::UNKNOWN, tokens.size(), lineNumber - 1, 0));
	tokenValues.emplace_back("");
}

void LexicalAnalyser::TokenizeLine() {
	for (int i = 0; i < tempLine.size(); i++) {
		currentLineIndex = i;
		if (lexicalAnalysisFailed)
			return;

		if (stringLiteral != '\0') {
			tempString += tempLine[i];
			if (tempLine[i] == stringLiteral) {
				stringLiteral = '\0';
				addToken();
			}
			continue;
		}

		switch (tempLine[i]) {
		case '\t':
		case ' ':
			addToken();
			break;

		case '#':
			addToken();
			return;

			// Punctuators
		case '(':
			addToken();
			tempString += tempLine[i];
			addPunctuatorToken(Token::OPEN_PARENTHESIS);
			break;
		case ')':
			addToken();
			tempString += tempLine[i];
			addPunctuatorToken(Token::CLOSE_PARENTHESIS);
			break;
		case '{':
			addToken();
			tempString += tempLine[i];
			addPunctuatorToken(Token::OPEN_CURLY);
			break;
		case '}':
			addToken();
			tempString += tempLine[i];
			addPunctuatorToken(Token::CLOSE_CURLY);
			break;
		case '[':
			addToken();
			tempString += tempLine[i];
			addPunctuatorToken(Token::PUNCTUATOR);
			break;
		case ']':
			addToken();
			tempString += tempLine[i];
			addPunctuatorToken(Token::PUNCTUATOR);
			break;
		case ';':
			addToken();
			tempString += tempLine[i];
			addPunctuatorToken(Token::END_STATEMENT);
			break;
		case ',':
			addToken();
			tempString += tempLine[i];
			addPunctuatorToken(Token::PUNCTUATOR);
			break;

			// Strings and characters
		case '\'':
		case '"':
			addToken();
			tempString += tempLine[i];
			stringLiteral = tempLine[i];
			stringBeginLine = lineNumber;
			break;

			// Operators
		case '+':
		case '-':
		case '*':
		case '/':
		case '<':
		case '>':
			addToken();
			tempString += tempLine[i];
			{
				int flags = (tempLine[i] == '<' || tempLine[i] == '>') ? Token::COMPARISON : Token::UNKNOWN;
				tokens.emplace_back(Token(Token::OPERATOR | flags, tokenValues.size(), lineNumber, currentLineIndex));
				tokenValues.emplace_back(tempString);
				tempString = "";
			}
			break;

		case '=':
			addToken();
			tempString += tempLine[i];
			{
				int flags = Token::ASSIGNMENT;
				if (i < tempLine.length() - 1) { // comparison operator
					if (tempLine[i + 1] == '=') {
						tempString += tempLine[i];
						flags = Token::COMPARISON;
						i++;
					}
				}
				tokens.emplace_back(Token(Token::OPERATOR | flags, tokenValues.size(), lineNumber, currentLineIndex));
				tokenValues.emplace_back(tempString);
				tempString = "";
			}
			break;

			// Identifiers and key words
		default:
			if ((tempLine[i] >= 'a' && tempLine[i] <= 'z') || (tempLine[i] >= 'A' && tempLine[i] <= 'Z') || (tempLine[i] >= '0' && tempLine[i] <= '9') || tempLine[i] == '_') {
				tempString += tempLine[i];
			}
			else {
				lexicalAnalysisFailed = true;
				error = "Unexpected symbol in line " + std::to_string(lineNumber);
				ipc_sourceManager.showError(error);
				ipc_sourceManager.showError(tempLine, i, -1);
				return;
			}
			
			break;
		}
	}
	addToken();
}

// Checks for identifiers, key words, constants and string literals
void LexicalAnalyser::addToken() {
	if (tempString == "") {
		return;
	}

	if (tempString[0] == '\'') { // a char
		if (tempString.size() == 3 && tempString[2] == '\'') {
			tokens.emplace_back(Token(Token::CONSTANT, tokenValues.size(), lineNumber, currentLineIndex - tempString.length() + 1));
			tokenValues.emplace_back(tempString);
		}
		else {
			lexicalAnalysisFailed = true;
			ipc_sourceManager.showError("Invalid character on line " + std::to_string(lineNumber));
			int errorIndex = tempLine.find(tempString);
			ipc_sourceManager.showError(tempLine, errorIndex, errorIndex + tempString.size());
		}
		tempString = "";
		return;
	}
	else if (tempString[0] == '"') { // a string
		tokens.emplace_back(Token(Token::LITERAL, tokenValues.size(), lineNumber, currentLineIndex - tempString.length() + 1));
		tokenValues.emplace_back(tempString);
		tempString = "";
		return;
	}

	if (tempString[0] >= '0' && tempString[0] <= '9') { // a number
		for (char character : tempString) {
			if (character < '0' || character > '9') {
				lexicalAnalysisFailed = true;
				ipc_sourceManager.showError("Unexpected character in line " + std::to_string(lineNumber));
				int errorIndex = tempLine.find(tempString);
				ipc_sourceManager.showError(tempLine, errorIndex, errorIndex + tempString.size());
				tempString = "";
				return;
			}
		}

		// valid constant
		tokens.emplace_back(Token(Token::CONSTANT, tokenValues.size(), lineNumber, currentLineIndex - tempString.length()));
		tokenValues.emplace_back(tempString);
		tempString = "";
		return;
	}

	// check if keyword
	for (const std::string& keyWord : keyWords) {
		if (keyWord == tempString) {
			int flags = Token::UNKNOWN;

			if (keyWord == "and")
				flags |= Token::BOOLEAN_AND;
			else if (keyWord == "or")
				flags |= Token::BOOLEAN_OR;
			else if (keyWord == "not")
				flags |= Token::BOOLEAN_NOT;
			else if (keyWord == "true" || keyWord == "false")
				flags |= Token::CONSTANT;

			tokens.emplace_back(Token(Token::KEYWORD | flags, tokenValues.size(), lineNumber, currentLineIndex - tempString.length()));
			tokenValues.emplace_back(tempString);
			tempString = "";
			return;
		}
	}

	tokens.emplace_back(Token(Token::IDENTIFIER, tokenValues.size(), lineNumber, currentLineIndex - tempString.length()));
	tokenValues.emplace_back(tempString);
	tempString = "";
}

void LexicalAnalyser::addPunctuatorToken(int tokenFlag) {
	tokens.emplace_back(Token(Token::PUNCTUATOR | tokenFlag, tokenValues.size(), lineNumber, currentLineIndex));
	tokenValues.emplace_back(tempString);
	tempString = "";
}

bool LexicalAnalyser::failed() const{
	return lexicalAnalysisFailed;
}

std::string LexicalAnalyser::tokenValue(const int tokenIndex) const{
	return tokenValues[tokens[tokenIndex].tokenValueIndex];
}

std::string LexicalAnalyser::tokenValue(const Token& token) const {
	return tokenValues[token.tokenValueIndex];
}

LexicalAnalyser::~LexicalAnalyser() {
}
