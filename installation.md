# Installation

## Install dependencies

* Install CMake:
Skip this step if OpenCV3.3 has been installed
```
sudo apt-get install cmake
```

* Download OpenCV3.3
```
wget https://github.com/opencv/opencv/archive/3.3.0.zip --no-check-certificate
unzip 3.3.0.zip
```

* Download "gen-pkg-config-pc"
```
wget ftp://ftp.openailab.net/tools/script/gen-pkg-config-pc.sh
chmod +x ./gen-pkg-config-pc.sh
```

* Install OpenCV3.3
```
cd opencv-3.3.0
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local/AID/opencv3.3.0 ..
sudo make install
```

* Configure The Libraries
```
sudo ~/gen-pkg-config-pc.sh /usr/local/AID
```

## CVGesture compile

```
cd CVGesture
make
```

## Camera live demo 
* Please ensure camera is connected

```
taskset -c 4,5 ./demo
```
