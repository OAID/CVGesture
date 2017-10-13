# Multi-hand Gesture Recognization Base on OpenCV
CVGesture is a project that is maintained by **OPEN** AI LAB, it uses OpenCV to recognize mult-hand geture. 

The release version is 0.1.0, is based on [Rockchip RK3399](http://www.rock-chips.com/plus/3399.html) Platform, target OS is Ubuntu 16.04. Can download the source code from [OAID/CVGesture](https://github.com/OAID/CVGesture)

# Haar Feature Traing
   * https://github.com/Sandeep-777/HandGestureDetection

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
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local/ ..
make -j4
sudo make install
sudo ldconfig
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
4) ./demo
5) Raise your right hand to make a **palm** or **fist** shape.
```

# Release History

### Version 0.1.0 - 2017-10-13 
   
  Initial version supports **palm** and **fist** recognization : 

# Issue Report
Encounter any issue, please report on [issue report](https://github.com/OAID/CVGesture/issues). Issue report should contain the following information :

*  The exact description of the steps that are needed to reproduce the issue 
* The exact description of what happens and what you think is wrong 
