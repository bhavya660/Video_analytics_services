# Base Image
FROM ubuntu:22.04

# Set environment variables to avoid interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Updating APT and Installing Dependencies
RUN apt-get update && apt-get install -y \
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
    libopencv-dev python3-opencv \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Create directory for getting configuration from deployment
RUN mkdir -p /config
# RUN mkdir -p /files

# Build streaming service application and remove source after build
WORKDIR /root/apps/
COPY files/yolov3-tiny.cfg /root/apps/
COPY files/yolov3.cfg /root/apps/
COPY files/yolov3-tiny.weights /root/apps/
COPY files/yolov3.weights /root/apps/
COPY inc/ inc/
COPY src/ src/
COPY Makefile .
COPY config/config.json /config/
COPY files/sample.mp4 /files/
RUN make clean
RUN make
RUN rm -rf src/ inc/ 
WORKDIR /root/apps/bin
RUN mv videoservice /root/apps/
WORKDIR /root/apps
RUN rm -rf bin Makefile

# Set environment variables
#ENV LD_LIBRARY_PATH "/root/apps/myapp/streaming-service/lib/lib:${LD_LIBRARY_PATH}"
#ENV GST_PLUGIN_PATH "/root/apps/myapp/gst-nvjpeg:${GST_PLUGIN_PATH}"
ENV GST_DEBUG="1"
ENV MALLOC_TRACE="/root/apps/myapp/"

#RUN export LD_LIBRARY_PATH
#RUN export GST_PLUGIN_PATH
RUN export GST_DEBUG
RUN export MALLOC_TRACE

# # Set streaming service application as entry point
#ENTRYPOINT ["sh", "-c",  "GST_DEBUG_DUMP_DOT_DIR=/streaming-data/ ./videoservice"]
# ENTRYPOINT ["sh", "-c",  "./videoservice"]
