#pragma once

#include "HttpTools.hpp"

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
	virtual ~ConfigBase() = 0;

	const std::string& getRoot() const;
	const std::string& getIndex() const;
	bool getAutoIndex() const;
	long getClientMaxBodySize() const;
	const std::map<int, std::string>& getErrorPages() const;
	const std::set<httpMethods>& getallowedMethods() const;
	const std::pair<int, std::string> &getReturnDirective() const;

	void setRoot(const std::string& src);
	void setIndex(const std::string& src);
	void setAutoIndex(bool src);
	void setClientMaxBodySize(long src);
	void setErrorPages(const std::map<int, std::string> &src);
	void setAllowedMethods(const std::set<httpMethods> &src);
    void setReturnDirective(const std::pair<int, std::string> &src);


};
