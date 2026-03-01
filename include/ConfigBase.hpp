#pragma once

#include "HttpTools.hpp"
#include "Tools.hpp"

#include <stdint.h>
#include <cstdlib>
#include <string>
#include <limits>
#include <cctype>
#include <map>
#include <set>

// #define MAX_K 4194303
// #define MAX_M 4095
// #define MAX_G 3

// #define FACTOR_K 1024
// #define FACTOR_M 1048576
// #define FACTOR_G 1073741824

typedef uint8_t bitmask_t;

struct UnitConversion {
	bitmask_t mask;
	unsigned int maxVal;
	unsigned int factor;
	std::size_t maxStrLen;
};

enum ValidChars {
	MASK_K = 1 << 0,
	MASK_M = 1 << 1,
	MASK_G = 1 << 2
};

class ConfigBase
{
private:
	std::string _root;
	std::vector<std::string> _index;
	bool _autoindex;
	long _clientMaxBodySize;
	std::map<int, std::string> _errorPages;
	std::set<std::string> _allowedMethods;
	std::pair<int, std::string> _returnDirective;

	static const bitmask_t _allowedBits = MASK_K | MASK_M | MASK_G;
	static const UnitConversion _conversionTable[];

	static const unsigned int MAX_K = 4194303;
	static const unsigned int MAX_M = 4095;
	static const unsigned int MAX_G = 3;

	static const unsigned int FACTOR_K = 1024;
	static const unsigned int FACTOR_M = 1048576;
	static const unsigned int FACTOR_G = 1073741824;

	static const unsigned int MAX_STR_K = 7;
	static const unsigned int MAX_STR_M = 4;
	static const unsigned int MAX_STR_G = 1;

public:
	ConfigBase &operator=(const ConfigBase &obj);
	virtual ~ConfigBase() = 0;

	const std::string& getRoot() const;
	const std::vector<std::string>& getIndex() const;
	bool getAutoIndex() const;
	long getClientMaxBodySize() const;
	const std::map<int, std::string>& getErrorPages() const;
	const std::set<std::string>& getAllowMethods() const;
	const std::pair<int, std::string> &getReturnDirective() const;

	void setRoot(const std::string& src);
	void setIndex(const std::vector<std::string>& src);
	void setAutoIndex(bool src);
	void setClientMaxBodySize(long src);
	void setErrorPages(const std::map<int, std::string> &src);
	void setAllowMethods(const std::set<std::string> &src);
    void setReturnDirective(const std::pair<int, std::string> &src);

	bool handleRoot(const std::vector<std::string>& tokens, std::ifstream& infile);
	bool handleIndex(const std::vector<std::string>& tokens, std::ifstream& infile);
	bool handleAutoindex(const std::vector<std::string>& tokens, std::ifstream& infile);
	bool handleClientMaxBodySize(const std::vector<std::string>& tokens, std::ifstream& infile);
	bool handleErrorPage(const std::vector<std::string>& tokens, std::ifstream& infile);
	bool handleAllowMethods(const std::vector<std::string>& tokens, std::ifstream& infile);
	bool handleReturn(const std::vector<std::string>& tokens, std::ifstream& infile);

	bool handleErrorOneLiner(const std::vector<std::string>& tokens);
	bool handleErrorMultiLiner(const std::vector<std::string>& tokens, std::ifstream& infile);
	bool handleMaxSizeConversion(std::string& maxSize);

	unsigned int expandMaskedString(std::string& src, bitmask_t foundBit);
	bitmask_t charToBit(char c);
};
