#include "Source manager.h"
#include <iostream>
#include <Windows.h>

void SourceCodeManager::addLine(std::string line) {
	sourceLines.push_back(line);
}

void SourceCodeManager::showError(std::string error) {
	showError(error, -1, -1);
}

void SourceCodeManager::showError(std::string error, int errorIndex, int errEndIndex) {
	if (errorIndex == -1) {
		std::cout << error << std::endl;
	}
	else {
		errEndIndex = (errEndIndex == -1) ? errorIndex + 1 : errEndIndex;
		for (int i = 0; i < error.size(); i++) {
			if (i >= errorIndex && i < errEndIndex) {
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hConsole, 0xC);
				std::cout << error[i];
				SetConsoleTextAttribute(hConsole, 7);
			}
			else {
				std::cout << error[i];
			}
		}
		std::cout << std::endl;
	}
}

void SourceCodeManager::showError(const Token& token) {
	std::string tokenString = ipc_lexer.tokenValues[token.tokenValueIndex];
	showError(sourceLines[token.tokenLineNumber - 1], token.tokenLineIndex, token.tokenLineIndex + tokenString.size());
}

void SourceCodeManager::errMissingSymbol(const Token& prevToken) {
	std::string tokenString = ipc_lexer.tokenValues[prevToken.tokenValueIndex];
	int insertIndex = prevToken.tokenLineIndex + tokenString.size();
	std::string errLine = sourceLines[prevToken.tokenLineNumber - 1];
	errLine.insert(insertIndex, 1, '_');
	showError(errLine, insertIndex, insertIndex + 1);
}