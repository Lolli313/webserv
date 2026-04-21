# WEBSERV

This project has been created as part
of the 42 curriculum by chboegne, tcardair, aakerblo.

## Description

The objective was to recreate the principle of a web server that works with request like nginx.

## Instructions

Too recreate this project, we use all c++98 standard function and those functions in C:

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

- 42 subject
- Mistral IA
- cppreference.com
- nginx.org

## Usage Exemples

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

Deleting a tweet or removing a file from cloud storage.

## Feature List

To launch the project, go to the terminal at the root of the project and use this command:

- make && ./webserv test.conf
make to compile the project, ./webserv to execute it and test.conf is configuration you can modify to use our webserv.

To verify if there is some issues in the code:
- make && valgrind --leak-check=full --trace-children=yes --track-fds=yes -s ./webserv test.conf

And then you can use those URl to try some request and cgi:
- http://localhost:7080/
- http://localhost:7081/cgi-bin/

be carefull 7080 is the number of the port we currently use. So if you change it you should change those url.

## Technical Choices

We add some CGI and cookies because it's fun, a lot fun.
