```
▄▄▌ ▐ ▄▌▄▄▄ .▄▄▄▄· .▄▄ · ▄▄▄ .▄▄▄   ▌ ▐·
██· █▌▐█▀▄.▀·▐█ ▀█▪▐█ ▀. ▀▄.▀·▀▄ █·▪█·█▌
██▪▐█▐▐▌▐▀▀▪▄▐█▀▀█▄▄▀▀▀█▄▐▀▀▪▄▐▀▀▄ ▐█▐█•
▐█▌██▐█▌▐█▄▄▌██▄▪▐█▐█▄▪▐█▐█▄▄▌▐█•█▌ ███ 
 ▀▀▀▀ ▀▪ ▀▀▀ ·▀▀▀▀  ▀▀▀▀  ▀▀▀ .▀  ▀. ▀  

     .'(   )\.---.     /(,-.    )\.--.   )\.---.     /`-.       .-. 
 ,') \  ) (   ,-._(  ,' _   )  (   ._.' (   ,-._(  ,' _  \  ,'  /  )
(  /(/ /   \  '-,   (  '-' (    `-.`.    \  '-,   (  '-' ( (  ) | ( 
 )    (     ) ,-`    )  _   )  ,_ (  \    ) ,-`    ) ,_ .'  ) './ / 
(  .'\ \   (  ``-.  (  '-' /  (  '.)  )  (  ``-.  (  ' ) \ (  ,  (  
 )/   )/    )..-.(   )/._.'    '._,_.'    )..-.(   )/   )/  )/..'   

              ___.                              
__  _  __ ____\_ |__   ______ ______________  __
\ \/ \/ // __ \| __ \ /  ___// __ \_  __ \  \/ /
 \     /\  ___/| \_\ \\___ \\  ___/|  | \/\   / 
  \/\_/  \___  >___  /____  >\___  >__|    \_/  
             \/    \/     \/     \/             
```
# WEBSERV
*A from scratch webserver implementing the HTTP/1.1 requirements and basic Nginx configurations.*

## Context
*This project has been created as part of the 42 curriculum by: chboegne, tcardair, aakerblo.*

## Description
A low-level web server that adheres to HTTP/1.1 requirements (implementing the GET, POST, DELETE methods), supports configuration files similar to Nginx, and integrates CGI handling. Particular emphasis was placed on building a stress-resistant system capable of maintaining 100% availability without crashing under load.

## Table of Contents
- [Context](#context)
- [Description](#description)
- [Usage](#usage)
  - [Installation](#installation)
  - [Prerequisites](#prerequisites)
  - [Execution](#execution)
  - [Configuration](#configuration)
- [Project constraints](#project-constraints)
- [Features](#features)
  - [GET](#get)
  - [POST](#post)
  - [DELETE](#delete)
  - [CGI (Common Gateway Interface)](#cgi-common-gateway-interface)
  - [Cookies](#cookies)
- [Resources](#resources)

## Usage

### Installation

First you will need to clone the repository on your computer
```
git clone https://github.com/Lolli313/webserv.git
```

Then, to compile the project, go at the root of the project and use this command:

```
make
```

This command compiles all of the source files into the executable *webserv*.

### Prerequisites

A configuration file `webserv.conf` is given by default, you can change it to add your custom configuration following the Nginx configuration file requirements.

As example, a default website is given. This can be changed and modified at will by adding your own in the `/files` directory.

### Execution

To start the web server, run the executable by providing a path to your configuration file:
Bash

```
./webserv <configuration file>
```

Once the server is initialized, it will begin listening for incoming connections based on the rules defined in your config. The most straightforward way to verify it's running is to open your browser and navigate to http://localhost:8080 or http://localhost:8081 (to use CGIs).

#### Manual Testing Options

Beyond basic browser navigation, you can use these tools to stress-test the server's robustness and inspect the raw data exchange:

- Telnet: For a low-level "handshake" test, use `telnet localhost 8080`. This allows you to manually type an HTTP request (e.g., GET / HTTP/1.1) and see the raw response headers and body exactly as the server sends them. 
> [!NOTE]
> The `Host` header is required for HTTP/1.1 request.

- Siege: To test how your server handles concurrency and high traffic, use Siege. Running a command like `siege -c 100 -t 1M http://localhost:8080` will simulate 100 simultaneous users for one minute, providing a report on availability and response times.

- Browser Network Tab: Press `F12` to open the Network Tab. This is invaluable for debugging status codes, verifying Keep-Alive behavior, and inspecting specific headers like Content-Type. You can create a custom request by pressing `+`.

### Configuration

The webserver is highly configurable, following the Nginx config file principle. [See more information here](CONFIG.md)

## Project constraints
This project has been achieved using only the standard library (c++98) and mostly system level functions:

execve, pipe, strerror, gai_strerror, errno, dup,
dup2, fork, socketpair, htons, htonl, ntohs, ntohl,
select, poll, epoll (epoll_create, epoll_ctl,
epoll_wait), kqueue (kqueue, kevent), socket,
accept, listen, send, recv, chdir, bind, connect,
getaddrinfo, freeaddrinfo, setsockopt, getsockname,
getprotobyname, fcntl, close, read, write, waitpid,
kill, signal, access, stat, open, opendir, readdir
and closedir

## Features

The project implements the GET, POST, DELETE methods and CGIs of the HTTP/1.1 protocol, as well as cookies.
### GET

**Purpose:** Retrieve data from a server

**How it works:**

* Requests data from a resource (e.g., webpage or API)
* Parameters are sent in the URL (query string)
* Data is visible and can be cached
* Not suitable for sensitive information

**Example:**

* Opening a webpage in your browser

---

### POST

**Purpose:** Send data to create or update a resource

**How it works:**

* Sends data in the request body (not the URL)
* Used for form submissions, file uploads, etc.
* Can trigger changes on the server (e.g., create a user)
* Safer for sensitive data than GET

**Example:**

* Submitting a login form
* Posting a comment

---

### DELETE

**Purpose:** Remove a resource from the server

**How it works:**

* Identifies the resource via the URL
* Performs a destructive action (deletion)
* Usually requires authentication

**Example:**

* Deleting a post or file

### CGI (Common Gateway Interface)

**Definition:**\
CGI is a standard protocol that allows web servers to execute external programs (scripts or applications) and generate dynamic content in response to client requests. It acts as an interface between the web server and external software, enabling the server to process user input (e.g., form submissions) and return customized output.

**Key Points:**\
CGI scripts are often written in languages like Perl, Python, or Bash.
Each request triggers a new process, which can impact performance for high-traffic sites.
CGI was widely used in early dynamic web applications but has largely been replaced by more efficient technologies like FastCGI, PHP, or server-side frameworks (e.g., Node.js, Django).

### Cookies
**Definition:**\
Cookies are small pieces of data stored on a user’s device by a web server. They are sent back to the server with each subsequent request, allowing the server to recognize the user, remember preferences, and maintain session state.

**Key Points:**\
Purpose: Track user sessions, personalize content, and store user preferences (e.g., login status, language settings).
Types:

Session Cookies: Temporary, deleted when the browser is closed.
Persistent Cookies: Remain on the device until they expire or are deleted.

Security: Cookies can be secured using attributes like HttpOnly (prevents JavaScript access) and Secure (transmitted only over HTTPS).

## Resources

- HTTP protocol
- RFC 7230 (HTTP/1.1)
- RFC 3875 (CGI/1.1)
- cppreference.com
- nginx.org
- youtube.com
- dev.to
- developer.mozilla.org
- geeksforgeeks.org
- medium.com
- linux man (functions' manual) 

