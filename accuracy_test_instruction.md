# Accuracy test instructions

## Install dependencies

* Install OpenCV library for Python
```
sudo cp /usr/local/AID/opencv3.3.0/lib/python2.7/dist-packages/cv2.so /usr/local/lib/python2.7/dist-packages/cv2.so
```

* Install Matplotlib for Python
```
sudo apt-get install python-matplotlib
```

## Run accuracy test script

```
1) cd CVGesture
2) python handmodel_test.py
```

## Add your own images to test dataset
```
1) Move your palm images to CVGesture/test_dataset/palm/img.
2) Move your fist images to CVGesture/test_dataset/fist/img.
3) Rename the images to palm_test+number.jpg or fist_test+number.jpg like palm_test1.jpg.
4) Install labelImg from https://github.com/tzutalin/labelImg.
5) Using labelImg to draw rectangle for your images (use "palm" and "fist" labels).
6) Save palm xml files to CVGesture/test_dataset/palm/annotation.
7) Save fist xml files to CVGesture/test_dataset/fist/annotation.
8) Run handmodel_test.py to start testing.
```
