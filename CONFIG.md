# Web Server Configuration

This configuration file defines the behavior of a custom web server, inspired by Nginx's syntax and logic.

---

## Overview

The server listens on port `7080` and handles requests for `localhost`. It includes rules for:
- Allowed HTTP methods
- Custom error pages
- Redirects and custom status codes

---

## Configuration Directives

### Global Settings
   Directive               | Description                                                                 | Example                     |
 |-------------------------|-----------------------------------------------------------------------------|-----------------------------|
 | `listen`                | IP and port the server listens on.                                          | `listen 0.0.0.0:7080;`      |
 | `server_name`           | Domain name or IP address for the server.                                   | `server_name localhost;`     |
 | `index`                 | Default file served when a directory is requested.                         | `index index.html;`          |
 | `autoindex`             | Enables or disables directory listing.                                      | `autoindex off;`             |
 | `client_max_body_size`  | Maximum allowed size for client request body.                               | `client_max_body_size 5M;`   |
 | `error_page`            | Custom error pages for specific HTTP status codes.                          | `error_page 404 /404.html;`  |

---

### Location Blocks
 | Directive       | Description                                                                 | Example                              |
 |-----------------|-----------------------------------------------------------------------------|--------------------------------------|
 | `location /`    | Rules for requests to the root path.                                        | `location / { allow_methods GET; }` |
 | `allow_methods` | Allowed HTTP methods for a specific path.                                   | `allow_methods GET POST PUT;`       |
 | `return`        | Defines a redirect or custom return code.                                  | `return 302 /;`                     |

---

### Custom Status Codes
 | Directive       | Description                                                                 | Example                     |
 |-----------------|-----------------------------------------------------------------------------|-----------------------------|
 | `return 578`    | Returns a custom HTTP status code.                                          | `return 578;`               |

---

## Example Configuration

```nginx
server {
    listen 0.0.0.0:7080;
    server_name localhost;

    index index.html;
    autoindex off;
    client_max_body_size 5M;

    error_page {
        403 /errors/403.html;
        404 /errors/404.html;
        500 /errors/500.html;
        502 /errors/502.html;
        504 /errors/504.html;
        505 /errors/505.html;
        578 /ascii/body.html;
    }

    location / {
        allow_methods GET POST PUT;
    }

    location /upload {
        allow_methods GET POST DELETE;
    }

    location /redirect {
        return 302 /;
    }

    location /youtube {
        return 302 https://youtube.com;
    }

    location /body {
        return 578;
    }
}