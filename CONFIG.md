# Custom Web Server Configuration

This document describes the configuration syntax for a custom web server inspired by Nginx. It supports static file serving, CGI execution, HTTP method restrictions, custom error pages, and redirects.

---

## Table of Contents
- [Global Directives](#global-directives)
- [CGI Configuration](#cgi-configuration)
- [Location Blocks](#location-blocks)
- [Example Configurations](#example-configurations)
- [Best Practices](#best-practices)

---

## Global Directives

| Directive               | Description                                                                 | Example                     |
|-------------------------|-----------------------------------------------------------------------------|-----------------------------|
| `listen`                | Binds the server to a specific IP and port.                                | `listen 0.0.0.0:7080;`      |
| `server_name`           | Defines the domain name or IP address for the server.                       | `server_name localhost;`     |
| `index`                 | Specifies the default file served when a directory is requested.           | `index index.html;`          |
| `autoindex`             | Enables or disables automatic directory listing.                          | `autoindex off;`             |
| `client_max_body_size`  | Sets the maximum allowed size for client request bodies.                   | `client_max_body_size 5M;`   |
| `error_page`            | Defines custom error pages for specific HTTP status codes.                 | `error_page 404 /404.html;`  |

---

## CGI Configuration

The `cgi` block configures CGI script execution. Allowed script languages are `python`, `php` and `shell`.

| Directive               | Description                                                                 | Example                     |
|-------------------------|-----------------------------------------------------------------------------|-----------------------------|
| `cgi`                   | Defines the path and interpreters for CGI scripts.                         | See example below.          |

**Example:**
```nginx
cgi {
    path /cgi-bin;
    python /usr/bin/python3;
    php /usr/bin/php;
    sh /bin/sh;
}
```

---

## Location Blocks

Location blocks define rules for specific paths by overriding the global directives. **Note:** `server_name` and `listen` are **not allowed** inside `location` blocks.

| Directive               | Description                                                                 | Example                     |
|-------------------------|-----------------------------------------------------------------------------|-----------------------------|
| `location /path`        | Applies rules to requests matching the specified path.                     | `location / { ... }`         |
| `allow_methods`         | Restricts allowed HTTP methods for a specific path.                       | `allow_methods GET POST;`   |
| `return`                | Defines redirects or custom HTTP status codes.                             | `return 302 /;`             |
| `autoindex`             | Overrides global `autoindex` settings for a specific path.                 | `autoindex on;`              |
| `index`                 | Overrides the global `index` setting for a specific path.                 | `index cgi.html;`           |

---

## Example Configurations

### Basic Server with Static Files
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
}
```

### Server with CGI Support
```nginx
server {
    listen 0.0.0.0:7081;
    server_name localhost;

    index index.html;
    autoindex off;

    cgi {
        path /cgi-bin;
        python /usr/bin/python3;
        php /usr/bin/php;
        sh /bin/sh;
    }

    location /cgi-bin {
        allow_methods GET POST;
        autoindex on;
        index cgi.html;
    }
}
```

---

## Best Practices

- **Security**: Restrict allowed HTTP methods and disable `autoindex` unless necessary.
- **Performance**: Adjust `client_max_body_size` based on your needs.
- **Maintainability**: Use `location` blocks to organize rules by path.
- **CGI**: Ensure CGI paths and interpreters are correctly configured and secured.
