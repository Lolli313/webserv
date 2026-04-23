# webserv

Test command:\
server
```
make && ./webserv webserv.conf
```
- browser client
```
localhost:8080
```

## Manual testing
you can use ``telnet`` to send handwritten commands.


### 3. test the nginx server
you can use a webrowser at ``localhost:8081``, or a terminal command such as
```
telnet localhost 8081
```

then you have to manually write the request, for example:
```
GET /ascii/body.txt HTTP/1.1
Host: localhost
```
then press `enter` 2x

### 4. siege

-c (number of clients)
-t (time before a client reconnects)
-r (number of attempts)

siege -c100 -t20s http://localhost:8080

### 5. Apache Bench
- sudo apt-get ab

