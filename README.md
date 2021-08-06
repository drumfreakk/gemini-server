# gemini-server

## Setup & Installation

### Requirements:
- [LibreSSL](https://www.libressl.org/)

### Compiling
Run `make`  to compile and create an executable at bin/gemini-server.  
Create a key and certificate with `openssl req -newkey rsa:4096 -new -nodes -x509 -days 3650 -keyout key.pem -out cert.pem` or a similar command, and fill in the correct data. The Common Name is the domain name your website will be hosted on.\
