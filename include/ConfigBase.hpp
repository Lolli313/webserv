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

// If getenv() doesn't find PWD for whatever reason (highly unlikely)
#define LAST_RESORT_PATH "/home/aakerblo/to_home/common_core/webserv/files"
#define FILE_FOLDER_NAME "/files"
#define DEFAULT_CLIENT_MAX_BODY_SIZE "5M"

typedef uint8_t bitmask_t;

/**
 * @brief Metadata for validating and scaling numeric strings with unit suffixes.
 * * This structure provides the paramenters required to convert a string (e.g., "2G") 
 * into its raw integer representation while preventing arithmetic overflows.
 */
struct UnitConversion
{
	// @brief Bitmask from ValidChars representing the allowed suffix (e.g., MASK_K)
	bitmask_t mask;

	// The highest numeric value allowed *before* multiplication.
	unsigned int maxVal;

	// The multiplier associated with the unit (e.g., 1024, 1048576).
	unsigned int factor;

	/** @brief The maximum allowed string length for the numeric part of the input.
     * Used for fast-fail validation before parsing the full string. */
	std::size_t maxStrLen;
};

/**
 * @brief Bitmask defining which size suffix characters are allowed.
 *
 * Each flag corresponds to a valid suffix character:
 * ```
 * |   Flag   | Character | Meaning   |
 * |----------|-----------|-----------|
 * | `MASK_K` |    `k`    | Kilobytes |
 * | `MASK_M` |    `m`    | Megabytes |
 * | `MASK_G` |    `g`    | Gigabytes |
 * ```
 * @attention case-insentive
 */
enum ValidChars
{
	NONE = 0,
	MASK_K = 1 << 0,
	MASK_M = 1 << 1,
	MASK_G = 1 << 2
};

/**
 * @brief Base class that holds variables for ServerBlockConfig,
 * LocationConfig and Server classes
 */
class ConfigBase
{
private:
	std::string _root;
	std::vector<std::string> _index;
	bool _autoindex;
	long _clientMaxBodySize;
	std::map<int, std::string> _errorPages; // map<error code, path>
	std::set<std::string> _allowedMethods;
	std::pair<int, std::string> _returnDirective; //

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
	ConfigBase();
	ConfigBase &operator=(const ConfigBase &obj);
	ConfigBase(const ConfigBase &obj);
	virtual ~ConfigBase() = 0;

	const std::string &getRoot() const;
	const std::vector<std::string> &getIndex() const;
	bool getAutoIndex() const;
	long getClientMaxBodySize() const;
	const std::map<int, std::string> &getErrorPages() const;
	const std::set<std::string> &getAllowMethods() const;
	const std::pair<int, std::string> &getReturnDirective() const;

	void setRoot(const std::string &src);
	void setIndex(const std::vector<std::string> &src);
	void setAutoIndex(bool src);
	void setClientMaxBodySize(long src);
	void setErrorPages(const std::map<int, std::string> &src);
	void setAllowMethods(const std::set<std::string> &src);
	void setReturnDirective(const std::pair<int, std::string> &src);

	bool handleRoot(std::vector<std::string> &tokens, std::ifstream *infile);
	bool handleIndex(std::vector<std::string> &tokens, std::ifstream *infile);
	bool handleAutoindex(std::vector<std::string> &tokens, std::ifstream *infile);
	bool handleClientMaxBodySize(std::vector<std::string> &tokens, std::ifstream *infile);
	bool handleErrorPage(std::vector<std::string> &tokens, std::ifstream *infile);
	bool handleAllowMethods(std::vector<std::string> &tokens, std::ifstream *infile);
	bool handleReturn(std::vector<std::string> &tokens, std::ifstream *infile);

	bool handleErrorOneLiner(std::vector<std::string> &tokens);
	bool handleErrorMultiLiner(std::vector<std::string> &tokens, std::ifstream *infile);
	void addOrReplaceErrorPage(int httpCode, const std::string& path);
	bool handleMaxSizeConversion(std::string &maxSize);

	unsigned int expandMaskedString(std::string &src, bitmask_t foundBit);
	bitmask_t charToBit(char c);

	void printData() const;
	void initWithDefaultData();
	void initRoot();
};
