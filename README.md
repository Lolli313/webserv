# webserv

Test commands:
- server
```
make && valgrind --leak-check=full --track-fds=yes ./webserv test.conf
```
- client
```
c++ backup/client_backup.cpp -o client && ./client
```