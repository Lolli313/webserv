#pragma once

/*
================================================================================
======= LIBRARIES ==============================================================
================================================================================
*/

#include <vector>
#include <string>
#include <cerrno>
#include <fcntl.h>
#include <netdb.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*
================================================================================
======= COLROS =================================================================
================================================================================
*/

#define RST "\033[0m"
#define RED "\033[1;31m"
#define GRN "\033[1;32m"
#define YEL "\033[1;33m"
#define BLU "\033[1;34m"
#define MAG "\033[1;35m"
#define CYN "\033[1;36m"
#define WHT "\033[1;37m"
