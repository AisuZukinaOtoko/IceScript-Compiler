#pragma once
#include <vector>
#include <string>
#include "Lexical Analyser.h"

class SourceCodeManager {
public:
	void addLine(std::string line);
	void showError(std::string error);
	void showError(std::string error, int startIndex, int endIndex);
	void showError(const Token& token);
	void errMissingSymbol(const Token& prevToken);
private:
	std::vector<std::string> sourceLines;
};
inline SourceCodeManager ipc_sourceManager;