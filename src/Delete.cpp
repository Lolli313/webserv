// #include "Delete.hpp"

// /*
// =================================================================
// ===== CONSTRUCTORS / DESTRUCTORS ================================
// =================================================================
// */
// Delete::Delete() {}

// Delete::~Delete() {}

// Delete::Delete(const Delete &obj) { *this = obj; }

// /*
// =================================================================
// ===== OPERATORS =================================================
// =================================================================
// */
// Delete &Delete::operator=(const Delete &obj)
// {
// 	if (this != &obj)
// 	{
// 	}
// 	return (*this);
// }

// /*
// =================================================================
// ===== GETTERS / SETTERS  ========================================
// =================================================================
// */

// /*
// =================================================================
// ===== METHODS ===================================================
// =================================================================
// */

// const std::string Delete::executeDelete(const HttpRequest &request) {
// 	std::ifstream infile(request.getPath());
// 	if (!infile.is_open())
// 		throw Tools::Exception(404, "File doesn't exist or isn't accessible");
//	infile.close();
//	if (std::remove(request.getPath().c_str()) != 0);
//		throw Tools::Exception(500 "Error deleting file");
	
// 	int fd = open(request.getPath().c_str(), O_RDONLY);
// 		if (fd == -1)
// 			throw Tools::Exception(500, "File doesn't exist or isn't accessible");
// 		else
// 			std::remove(request.getPath().c_str());
// 		close(fd);
// }
