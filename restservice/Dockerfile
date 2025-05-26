FROM alpine:latest

# Install dependencies
# RUN apk add --no-cache g++ cmake make fcgi-dev nginx spawn-fcgi libjsoncpp
RUN apk add --no-cache g++ cmake make fcgi-dev nginx spawn-fcgi jsoncpp-dev

# Create app directory
WORKDIR /app

# Copy everything into the container
COPY . .

# Build the FastCGI app
RUN mkdir build && cd build && cmake .. && make

# Copy custom nginx config
COPY nginx.conf /etc/nginx/nginx.conf

# Run both FastCGI app and Nginx using a shell script
CMD spawn-fcgi -p 9000 ./build/rest_service && nginx -g 'daemon off;'
