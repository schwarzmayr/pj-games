# Install SDL3 on Ubuntu 22.04 / Zorin OS 17

SDL3 is not available in the default Ubuntu 22.04 repositories, so we need to build it from source.

## Step 1: Install build dependencies

```bash
sudo apt-get update
sudo apt-get install -y build-essential git cmake ninja-build pkg-config \
    libasound2-dev libpulse-dev libaudio-dev libfribidi-dev libjack-dev libsndio-dev \
    libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev \
    libxss-dev libxtst-dev libxkbcommon-dev libdrm-dev libgbm-dev \
    libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev \
    libdbus-1-dev libibus-1.0-dev libudev-dev libthai-dev libusb-1.0-0-dev \
    libpipewire-0.3-dev libwayland-dev libdecor-0-dev liburing-dev
```

## Step 2: Clone and build SDL3

```bash
cd /tmp
git clone https://github.com/libsdl-org/SDL.git SDL3
cd SDL3
mkdir build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
ninja
sudo ninja install
sudo ldconfig
```

## Step 3: Verify installation

```bash
pkg-config --modversion sdl3
```

This should print the SDL3 version (e.g., `3.5.0`).