# WEBSERV
*A from scratch webserver implementing the HTTP/1.1 requirements and basic Nginx configurations.*


## Context
This project has been fully developped and conceived by chboegne, tcardair and aakerblo as part of the 42 curriculum.

## Description
A low-level web server that adheres to HTTP/1.1 requirements (implementing the GET, POST, DELETE methods), supports configuration files similar to Nginx, and integrates CGI handling. Particular emphasis was placed on building a stress-resistant system capable of maintaining 100% availability without crashing under load.

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

As example, a default website is given. This can be changed and modified as will by adding your own in the `/files` directory.

### Execution

To start the web server, use the following command:

```
./webserv <config file>
```

### Configuration

Our webserver is highly configurable. See more information [here](CONFIG.md)

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
- man

## Features

The only methods we recreate is GET, POST and DELETE.

### GET

- Purpose:

Retrieve data from a server.
- How it works:

Used to request data from a specified resource (e.g., a webpage, API endpoint).
Data is sent via the URL (as query parameters), making it visible and cacheable.
Should not be used to send sensitive information (like passwords).
- Example:

When you type a URL in your browser, you’re sending a GET request to fetch that webpage.

### POST

- Purpose:

Send data to a server to create or update a resource.
- How it works:

Used to submit data to be processed (e.g., form submissions, uploading files).
Data is sent in the request body, not the URL, making it more secure for sensitive information.
Can trigger side effects on the server (e.g., creating a new user account).
- Example:

Submitting a login form or posting a comment on social media.

### DELETE

- Purpose:

Remove a resource from the server.
- How it works:

Used to delete a specified resource (e.g., a user account, a file).
Like GET, the target is identified by the URL, but the action is destructive.
Often requires authentication to prevent unauthorized deletions.
- Example:

Deleting a tweet or removing a file from a storage.

## Feature List

To launch the project, go to the terminal at the root of the project and use this command:

- make && ./webserv webserv.conf
make to compile the project, ./webserv to execute it and test.conf is configuration you can modify to use our webserv.

To verify if there is some issues in the code:
- make && valgrind --leak-check=full --trace-children=yes --track-fds=yes -s ./webserv test.conf

And then you can use those URl to try some request and cgi:
- http://localhost:7080/
- http://localhost:7081/cgi-bin/

be carefull 7080 is the number of the port we currently use. So if you change it you should change those url.

## Technical Choices

We add some CGI and cookies because it's fun, a lot fun.

### CGI (Common Gateway Interface)
- Definition:

CGI is a standard protocol that allows web servers to execute external programs (scripts or applications) and generate dynamic content in response to client requests. It acts as an interface between the web server and external software, enabling the server to process user input (e.g., form submissions) and return customized output.
- Key Points:

CGI scripts are often written in languages like Perl, Python, or Bash.
Each request triggers a new process, which can impact performance for high-traffic sites.
CGI was widely used in early dynamic web applications but has largely been replaced by more efficient technologies like FastCGI, PHP, or server-side frameworks (e.g., Node.js, Django).

### Cookies
- Definition:

Cookies are small pieces of data stored on a user’s device by a web server. They are sent back to the server with each subsequent request, allowing the server to recognize the user, remember preferences, and maintain session state.
- Key Points:

Purpose: Track user sessions, personalize content, and store user preferences (e.g., login status, language settings).
Types:

Session Cookies: Temporary, deleted when the browser is closed.
Persistent Cookies: Remain on the device until they expire or are deleted.

Security: Cookies can be secured using attributes like HttpOnly (prevents JavaScript access) and Secure (transmitted only over HTTPS).


