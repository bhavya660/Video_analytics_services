# -------- Stage 1: Build the binary --------
FROM ubuntu:20.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive
ENV GOLANG_VERSION=1.22.0
ENV GOTOOLCHAIN=local

# Install dependencies
RUN apt-get update && apt-get install -y \
    git \
    curl \
    make \
    ca-certificates \
    build-essential \
 && rm -rf /var/lib/apt/lists/*

# Install Go manually (version 1.22)
RUN curl -LO https://go.dev/dl/go${GOLANG_VERSION}.linux-amd64.tar.gz && \
    tar -C /usr/local -xzf go${GOLANG_VERSION}.linux-amd64.tar.gz && \
    rm go${GOLANG_VERSION}.linux-amd64.tar.gz

ENV PATH="/usr/local/go/bin:${PATH}"

WORKDIR /build

# Clone and build specific version
RUN git clone https://github.com/bluenviron/mediamtx.git . && \
    git checkout v1.9.1 && \
    echo "v1.9.1" > VERSION && \
    mkdir -p internal/servers/hls && \
    curl -L -o internal/servers/hls/hls.min.js https://cdn.jsdelivr.net/npm/hls.js@1.4.10/dist/hls.min.js && \
    go build -o mediamtx .

# -------- Stage 2: Runtime container --------
FROM ubuntu:20.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    ca-certificates \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the built binary and mediamtx.yml from the builder stage
COPY --from=builder /build/mediamtx .
COPY ./mediamtx.yml /app/mediamtx.yml

# Expose the required ports
EXPOSE 8554/tcp
EXPOSE 1935/tcp
EXPOSE 8888/tcp
EXPOSE 8889/tcp
EXPOSE 8189/udp

# Run the mediamtx binary with the custom mediamtx.yml configuration file
ENTRYPOINT ["./mediamtx"]

