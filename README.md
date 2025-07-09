# Simple HTTP Web Server
This is a simple HTTP server dedicated to learning about networking, HTTP protocol, socket programming, operating systems, and the C language.

## Environment
- Currently you must open the application in the Docker container if you are not currently operating within a Linux distro
- Despite a Linux kernel being the only requirement to run the application, for non-Linux users and future proofing where there might be more dependencies, this is the current setup

## Build and Run
- After opening the repo in the Docker container, you can use make to compile the program
- To run the program, use the ./build/server command
- This starts the server

## Testing
- To test the server, go to (http://localhost:8080/index.html) in the browser while the process is running
- If you want to change the test contents, they are contained in webroot

## Features
This server currently supports basic HTTP GET requests for the 7 main MIME types:
- HTML, CSS, JS, JSON, JPG, PNG, TXT