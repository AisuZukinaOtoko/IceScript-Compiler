#pragma once
#include <vector>
#include <string>

struct Symbol {
	std::string id;
	std::string type;
	double value;
	bool initWarning = true;
};

class SymbolTable {
public:
	SymbolTable();
	~SymbolTable();
	//int symbolFindAdd(std::string symbol);
	bool addSymbol(std::string symbol);
	bool addSymbol(Symbol symbol);
	void printSymbols();
	//bool validSymbol(std::string symbol);
	//void setSymbolValue(std::string symbol, double value);
	//double getSymbolValue(std::string symbol);
	Symbol* getSymbol(std::string symbol);
private:
	std::string cachedSymbolId;
	int cachedIndex = -1;
	std::vector<Symbol> symbols;

	//int numWarnings = 0;
	//int maxWarningCount = 10;
};

inline SymbolTable ipc_symbolTable;