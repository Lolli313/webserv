#include "../include/ConfigBase.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

ConfigBase::~ConfigBase() {};

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

const std::string &ConfigBase::getRoot() const { return _root; }
const std::string &ConfigBase::getIndex() const { return _index; }
bool ConfigBase::getAutoIndex() const { return _autoindex; }
long ConfigBase::getClientMaxBodySize() const { return _clientMaxBodySize; }
const std::map<int, std::string> &ConfigBase::getErrorPages() const { return _errorPages; }
const std::set<httpMethods> &ConfigBase::getallowedMethods() const { return _allowedMethods; }
const std::pair<int, std::string> &ConfigBase::getReturnDirective() const { return _returnDirective; }

void ConfigBase::setRoot(const std::string& src){ _root = src; }
void ConfigBase::setIndex(const std::string& src){ _index = src; }
void ConfigBase::setAutoIndex(bool src){ _autoindex = src; }
void ConfigBase::setClientMaxBodySize(long src){ _clientMaxBodySize = src; }
void ConfigBase::setErrorPages(const std::map<int, std::string> &src){ _errorPages = src; }
void ConfigBase::setAllowedMethods(const std::set<httpMethods> &src){ _allowedMethods = src; }
void ConfigBase::setReturnDirective(const std::pair<int, std::string> &src) { _returnDirective = src; }

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

