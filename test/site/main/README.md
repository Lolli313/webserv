# Aero Webserver - Test Suite Guide

## Quick Start for Evaluators

### 1. Compile the Server
```bash
make
```

### 2. Run the Test Suite Server
```bash
./webserv config/test_suite.aero
```

### 3. Open Browser
Navigate to: **http://localhost:3000**

## What You'll Find

The test suite provides an interactive dashboard to test all webserver features:

### ✅ Core Features (Mandatory)
- **File Upload** - Test multipart/form-data POST requests
- **Static Files** - Serve HTML, CSS, JS, images
- **HTTP Methods** - GET, POST, DELETE, HEAD
- **Error Handling** - 404, 403, 413, 500, 505
- **CGI Execution** - Python/PHP script execution
- **Multiple Ports** - Different content on different ports
- **Redirections** - 301/302 redirects

### 🌟 Bonus Features
- **Cookies & Sessions** - Cookie parsing and session management
- **Multiple CGI** - Support for Python, PHP, etc.

## Manual Testing with curl

### Upload a file:
```bash
curl -F "file=@yourfile.txt" http://localhost:3000/uploads
```

### Delete a file:
```bash
curl -X DELETE http://localhost:3000/uploads/yourfile.txt
```

### Test error codes:
```bash
curl -I http://localhost:3000/nonexistent  # 404
curl -X POST http://localhost:3000/        # 405
```

## Stress Testing with Siege

### Install siege:
```bash
brew install siege  # macOS
sudo apt install siege  # Linux
```

### Run stress test:
```bash
siege -b -t30s http://localhost:3000/
```

**Expected Results:**
- Availability: > 99.5%
- No crashes
- No memory leaks
- No hanging connections

## Directory Structure

```
test/
├── test_suite/          # Test suite web interface
│   ├── index.html       # Main dashboard
│   └── *-test.html      # Individual test pages
└── www/                 # Static content & uploads
    ├── index.html       # Example upload page
    └── uploads/         # Upload directory
```

## Configuration Files

- `config/test_suite.aero` - Test suite config (port 3000)
- `config/static.aero` - Static site config
- `config/subject.aero` - Subject example config

## Evaluation Checklist

✓ Server compiles without errors  
✓ Configuration file parsing works  
✓ Multiple servers on different ports  
✓ GET, POST, DELETE methods work  
✓ File uploads save correctly  
✓ Error pages display properly  
✓ CGI scripts execute  
✓ No crashes or hangs  
✓ select() (or equivalent) used correctly  
✓ No memory leaks  
✓ Siege stress test passes  

## Common Issues

**Port already in use:**
```bash
lsof -ti:3000 | xargs kill -9
```

**Permission denied:**
```bash
chmod +x webserv
chmod 755 test/www/uploads/
```

## Contact

For questions during evaluation, ask the team!

---
**Aero Webserver** - C++ 98 HTTP/1.1 Implementation
