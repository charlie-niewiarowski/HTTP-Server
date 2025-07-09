# Simple HTTP Web Server
This is a simple HTTP server dedicated to learning about networking, socket programming, operating systems, and the C language.

## Environment

- Currently you must open the application in the Docker container, which is enforced by the Makefile only being usable within the container.
- Despite a Linux kernel being the only requirement to run the application, for future proofing when there might be more dependencies, this is the current setup.

## Build and Run

- After opening the repo in the Docker container, you can use make to compile the program.
To run the program, use the ./build/server command
- This starts the server, and to test it, simply go to (http://localhost:8080/test.txt) in the browser
This will open a test file of the Gettysburg Address

# Features
This server currently supports basic HTTP GET requests for the 7 main MIME types:
- HTML, CSS, JS, JSON, JPG, PNG, TXT