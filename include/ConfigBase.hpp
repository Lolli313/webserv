#pragma once

#include "HttpTools.hpp"
#include "Tools.hpp"

#include <cstdlib>
#include <string>
#include <map>
#include <set>

class ConfigBase
{
private:
	std::string _root;
	std::string _index;
	bool _autoindex;
	long _clientMaxBodySize;
	std::map<int, std::string> _errorPages;
	std::set<httpMethods> _allowedMethods;
	std::pair<int, std::string> _returnDirective;

public:
	ConfigBase &operator=(const ConfigBase &obj);
	virtual ~ConfigBase() = 0;

	const std::string& getRoot() const;
	const std::string& getIndex() const;
	bool getAutoIndex() const;
	long getClientMaxBodySize() const;
	const std::map<int, std::string>& getErrorPages() const;
	const std::set<httpMethods>& getAllowMethods() const;
	const std::pair<int, std::string> &getReturnDirective() const;

	void setRoot(const std::string& src);
	void setIndex(const std::string& src);
	void setAutoIndex(bool src);
	void setClientMaxBodySize(long src);
	void setErrorPages(const std::map<int, std::string> &src);
	void setAllowMethods(const std::set<httpMethods> &src);
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
};
