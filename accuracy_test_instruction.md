# Accuracy test instructions

## Install dependencies:

* Install OpenCV library for Python
```
sudo cp /usr/local/AID/opencv3.3.0/lib/python2.7/dist-packages/cv2.so /usr/local/lib/python2.7/dist-packages/cv2.so
```

* Install Matplotlib for Python:
```
sudo apt-get install python-matplotlib
```

## Run accuracy test script

```
1) cd CVHandGestureDetection
2) python handmodel_test.py
```

## Add your own images to test dataset
```
1) move your palm images to CVHandGestureDetection/test_dataset/palm/img
2) move your fist images to CVHandGestureDetection/test_dataset/fist/img
3) rename the images to palm_test+number.jpg or fist_test+number.jpg like palm_test1.jpg
4) install labelImg from https://github.com/tzutalin/labelImg
5) using labelImg to draw rectangle for your images (use "palm" and "fist" labels) 
6) save palm xml files to CVHandGestureDetection/test_dataset/palm/annotation
7) save fist xml files to CVHandGestureDetection/test_dataset/fist/annotation
8) run handmodel_test.py to start testing
```
