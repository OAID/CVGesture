# Installation

## Install dependencies:

* Install CMake:
Skip this step if OpenCV3.3 has been installed
```
sudo apt-get install cmake
```

* Install OpenCV3.3
```
wget https://github.com/opencv/opencv/archive/3.3.0.zip --no-check-certificate
unzip 3.30.zip
cd opencv-3.3.0
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local/AID/opencv3.3.0 ..
make -j4
sudo make install
sudo ldconfig
```

* Configure The Libraries:
Skip this step if OpenCV3.3 has been installed
```
wget ftp://ftp.openailab.net/tools/script/gen-pkg-config-pc.sh
chmod +x ./gen-pkg-config-pc.sh
sudo ~/gen-pkg-config-pc.sh /usr/local/AID
```

## CVGesture Compile

```
cd CVGesture
make
```

## Camera live demo 
```
1) cd CVGesture
2) make 
3) Plugin USB camera device
4) taskset -c 4,5 ./demo
5) Raise your right hand to make a **palm** or **fist** shape.
```
