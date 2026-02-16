#include "Tools.hpp"

/*
=================================================================
===== CONSTRUCTORS / DESTRUCTORS ================================
=================================================================
*/
Tools::Exception::Exception() {};
Tools::Exception::Exception(int returnCode, const std::string &msgLog) : _returnCode(returnCode), _msgLog(msgLog) {}
Tools::Exception::Exception(const std::string &msgLog) : _returnCode(0), _msgLog(msgLog) {}
Tools::Exception::Exception(const Exception &obj) { *this = obj; };
Tools::Exception::~Exception() throw() {}

/*
=================================================================
===== OPERATORS =================================================
=================================================================
*/
Tools::Exception &Tools::Exception::operator=(const Exception &obj)
{
	if (this != &obj)
	{
		this->_msgLog = obj._msgLog;
		this->_returnCode = obj._returnCode;
	}
	return (*this);
};

/*
=================================================================
===== GETTERS / SETTERS  ========================================
=================================================================
*/

int Tools::Exception::getReturnCode() const { return _returnCode; }

const std::string &Tools::Exception::getMsgLog() const { return _msgLog; }

/*
=================================================================
===== METHODS ===================================================
=================================================================
*/
