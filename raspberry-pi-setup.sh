#!/usr/bin/env bash

# dependencies
sudo apt-get install -y vim htop yasm libavcodec56 libavcodec-dev lame libmp3lame-dev libx264-dev libswscale-dev libavfilter5 libavfilter-dev libvorbis-dev libvorbis0a libvorbisenc2 libtheora-bin libtheora-dev libspeex1 libspeex-dev libc6-dev libopenjpeg-dev libpostproc52 libpostproc-dev gstreamer1.0 gstreamer1.0-tools gstreamer1.0-omx libgstreamer1.0-dev 

mkdir -p /usr/src
sudo chmod 777 /usr/src

# ffmpeg
# cd /usr/src
# git clone git://source.ffmpeg.org/ffmpeg.git
# cd ffmpeg
# ./configure --prefix=/opt/ffmpeg --arch=armel --target-os=linux --enable-nonfree --enable-gpl --enable-shared --enable-libx264 --enable-libmp3lame --enable-swscale --enable-avfilter --enable-libvorbis --enable-libtheora --enable-libspeex --enable-pthreads --enable-libopenjpeg --enable-postproc   
# make -j4
# sudo make install

cd /usr/src
wget https://gstreamer.freedesktop.org/src/gst-rtsp-server/gst-rtsp-server-1.4.4.tar.xz
tar xvf gst-rtsp-server-1.4.4.tar.xz
cd gst-rtsp-server-1.4.4
./configure --prefix=/opt/gst-rtsp
make
sudo bash -c "echo bcm2835-v4l2 >> /etc/modules"
sudo modprobe bcm2835-v4l2

echo "Now execute 'sudo raspi-config' and enable the camera and increase memory allocated to GPU (if required)"