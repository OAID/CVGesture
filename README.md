# Multi-hand Gesture Recognization Base on OpenCV
[![GitHub license](http://dmlc.github.io/img/apache2.svg)](./LICENSE)

**CVGesture**, developed by **OPEN** AI LAB, is an open source application that uses **OpenCV** (Open Source Computer Vision Library) to detect and recognize different hand gestures. As it does not use deep learning algorithms, it can achieve real-time detection (>=15 fps with A72x2 cores) and recognition while preserving high accuracy.

- Cascade classifiers based on Haar feature are trained to recognize different gestures. The object detector has been initially proposed by [Paul Viola](https://docs.opencv.org/2.4/modules/objdetect/doc/cascade_classification.html#viola01) and improved by [Rainer Lienhart](https://docs.opencv.org/2.4/modules/objdetect/doc/cascade_classification.html#lienhart02). Using Haar feature achieves higher recognition accuracy than using LBP feature.

- Current version of CVGesture recognizes two different gestures: palm and fist. Palm means hand facing camera with five fingers open (in some countries this gesture stands for “five”). Fist means hand facing camera with five fingers clenched. **The best distance between hand and camera is within 1m**.

- Multiple gestures can be recognized at the same time if there are several hands appearing in the camera. Theoretically there is no upper limit for detection and recognition of the amount of hands at the same time and more than one hand appearing in the camera will not slow down the recognition speed significantly.


The release version is 0.3.0, is based on [Rockchip RK3399](http://www.rock-chips.com/plus/3399.html) Platform, target OS is Ubuntu 16.04. You can download the source code from [OAID/CVGesture](https://github.com/OAID/CVGesture)

### Documents
* [Installation instructions](installation.md)
* [Accuracy test instructions](accuracy_test_instruction.md)
* [Performance Report PDF](performance_report.pdf)

# Haar Feature Traing
   * https://github.com/Sandeep-777/HandGestureDetection

# Release History

### Version 0.3.0 - 2018-01-25 
   
  Add script to test accuracy of classifier: 

### Version 0.2.0 - 2017-12-25 
   
  Retrained Haar cascade classifier to increase the performance: 
  
### Version 0.1.0 - 2017-10-13 
   
  Initial version supports **palm** and **fist** recognization : 

# Issue Report
Encounter any issue, please report on [issue report](https://github.com/OAID/CVGesture/issues). Issue report should contain the following information :

*  The exact description of the steps that are needed to reproduce the issue 
* The exact description of what happens and what you think is wrong 

  
