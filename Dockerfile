# Use an official lightweight Linux image with build tools
FROM debian:bullseye

# Install required packages: gcc, make, etc.
RUN apt-get update && \
    apt-get install -y build-essential

# Set the working directory inside the container
WORKDIR /app

# Copy your server source code into the container
COPY . .

# Build the server using my makefile
RUN make

# Expose the server port
EXPOSE 8080

# Run the server
CMD ["./server"]
