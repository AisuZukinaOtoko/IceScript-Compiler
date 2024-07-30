#include "Symbol Table.h"
#include "Source manager.h"
#include <iostream>

//int SymbolTable::symbolFindAdd(std::string searchSymbol) {
//	for (int i = 0; i < symbols.size(); i++) {
//		if (symbols[i] == searchSymbol) {
//			return i;
//		}
//	}
//	symbols.push_back(searchSymbol);
//	return symbols.size() - 1;
//}
SymbolTable::SymbolTable() {
	Symbol temp;
	temp.id = "true";
	temp.value = 1;
	temp.initWarning = false;
	addSymbol(temp);

	temp.id = "false";
	temp.value = 0;
	temp.initWarning = false;
	addSymbol(temp);
}

bool SymbolTable::addSymbol(Symbol symbol) {
	Symbol* sym = getSymbol(symbol.id);
	if (sym)
		return false;

	symbols.push_back(symbol);
	return true;
}


bool SymbolTable::addSymbol(std::string symbol) {
	Symbol* sym = getSymbol(symbol);
	if (sym)
		return false;

	Symbol temp;
	temp.id = symbol;
	symbols.push_back(temp);
	return true;
}

//bool SymbolTable::validSymbol(std::string symbol) {
//
//}

//double SymbolTable::getSymbolValue(std::string symbol) {
//	if (cachedSymbolId == symbol) {
//		return symbols[cachedIndex].value;
//	}
//
//	for (int i = 0; i < symbols.size(); i++) {
//		if (symbols[i].id == symbol) {
//			cachedSymbolId = symbols[i].id;
//			cachedIndex = i;
//			return symbols[i].value;
//		}
//	}
//	return 0;
//}

Symbol* SymbolTable::getSymbol(std::string symbol) {
	if (cachedSymbolId == symbol) {
		return &symbols[cachedIndex];
	}

	for (int i = 0; i < symbols.size(); i++) {
		if (symbols[i].id == symbol) {
			cachedSymbolId = symbols[i].id;
			cachedIndex = i;
			return &symbols[i];
		}
	}

	return nullptr;
}

void SymbolTable::printSymbols() {
	for (const Symbol& symbol : symbols) {
		if (symbol.id == "true" || symbol.id == "false")
			continue;
		std::cout << symbol.id << ": " << symbol.value << std::endl;
	}
}
//void SymbolTable::setSymbolValue(std::string symbol, double value) {
//	if (cachedSymbolId == symbol) {
//		symbols[cachedIndex].value = value;
//		return;
//	}
//
//	for (int i = 0; i < symbols.size(); i++) {
//		if (symbols[i].id == symbol) {
//			symbols[i].value = value;
//			cachedSymbolId = symbols[i].id;
//			cachedIndex = i;
//		}
//	}
//}

SymbolTable::~SymbolTable() {

}