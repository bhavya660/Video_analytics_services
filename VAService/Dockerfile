FROM gcc:13 AS builder

RUN apt-get update
RUN apt-get install -y \
    gstreamer1.0-tools \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav \
    gstreamer1.0-x \
    gstreamer1.0-alsa \
    gstreamer1.0-pulseaudio \
    libgstreamer1.0-dev \
    libjsoncpp-dev \
    cmake \
    g++ \
    libjsoncpp-dev \
    libopencv-dev python3-opencv \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .
RUN mkdir build && cd build && cmake .. && make

CMD ["/bin/bash"]
