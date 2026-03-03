#include "../include/HttpRequest.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/

// 1. Parse the request.
// 2. Analyse and throw errors accordingly if found.
HttpRequest::HttpRequest(const std::string &request)
{
  (void)request;
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
