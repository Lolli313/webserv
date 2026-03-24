# webserv

Test commands:
- server
```
make && valgrind --leak-check=full --track-fds=yes ./webserv test.conf
```
- terminal client
```
c++ backup/client_backup.cpp -o client && ./client
```

- browser client
```
localhost:8080
```

## Manual testing
you can use ``telnet`` to send handwritten commands.


## nginx
To compare our webserv with nginx (the soft we'r trying to mimic).

### 1. install nginx if it's not done.
```
docker pull nginx
```

### 2. start the nginx server
```
docker run --rm \
  -p 8081:80 \
  -v "$(pwd)/files:/usr/share/nginx/html:ro" \
  -v "$(pwd)/nginx.conf:/etc/nginx/nginx.conf:ro" \
  nginx:latest
```

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
