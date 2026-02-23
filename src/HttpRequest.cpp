#include "../include/HttpRequest.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

// 1. Parse the request.
// 2. Analyse and throw errors accordingly if found.
HttpRequest::HttpRequest(const std::string &request) {

};

HttpRequest::~HttpRequest() {};

HttpRequest::HttpRequest(const HttpRequest &obj) { *this = obj; };

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
HttpRequest &HttpRequest::operator=(const HttpRequest &obj)
{
  if (this != &obj)
  {
  }
  return (*this);
};

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/

bool HttpRequest::hasBody() { return !_body.empty(); }

// Split an input (string) according to Separator (can be a string as well)
std::vector<std::string> splitString(const std::string &input, const std::string &separator)
{
  std::vector<std::string> returnVector;

  std::string::size_type start = 0;
  std::string::size_type pos;

  while ((pos = input.find(separator, start) != std::string::npos))
  {
    returnVector.push_back(input.substr(start, pos - start));
    start = pos + separator.size();
  }
  returnVector.push_back(input.substr(start));
  return (returnVector);
}
