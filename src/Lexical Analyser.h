 #pragma once
#include <vector>
#include <string>

struct Token {
	enum Type {
		UNKNOWN = 0, IDENTIFIER, CONSTANT, OPERATOR, KEYWORD, LITERAL, PUNCTUATOR = 0x2000,
		END_STATEMENT = 0x8, OPEN_PARENTHESIS = 0x10, CLOSE_PARENTHESIS = 0x20,
		OPEN_CURLY = 0x40, CLOSE_CURLY = 0x80, COMPARISON = 0x100, ASSIGNMENT = 0x200,
		BOOLEAN_AND = 0x400, BOOLEAN_OR = 0x800, BOOLEAN_NOT = 0x1000
	};
	int tokenType = UNKNOWN;
	int tokenValueIndex = -1;
	int tokenLineNumber = 1;
	int tokenLineIndex = 0;

	Token() {
		tokenType = UNKNOWN;
		tokenValueIndex = -1;
	}
	Token(int type, int index, int lineNumber, int lineIndex) {
		tokenType = type;
		tokenValueIndex = index;
		tokenLineNumber = lineNumber;
		tokenLineIndex = lineIndex;
	}
};

class LexicalAnalyser {
public:
	LexicalAnalyser();
	~LexicalAnalyser();

	void tokenize(const char* file);
	bool failed() const;
	std::string tokenValue(const int tokenIndex) const;
	std::string tokenValue(const Token& token) const;
	std::vector<Token> tokens;
	std::vector<std::string> tokenValues;
private:
	std::vector<std::string> keyWords;
	void TokenizeLine(); // line by line for debugging
	void addToken();
	void addPunctuatorToken(int tokenFlag);

	int lineNumber = 1;
	int currentLineIndex = 0;
	int stringBeginLine = -1;
	std::string tempString;
	std::string tempLine;
	char stringLiteral = '\0';

	bool lexicalAnalysisFailed = false;
	std::string error;
};

inline LexicalAnalyser ipc_lexer;