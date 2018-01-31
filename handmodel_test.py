import cv2
import xml.dom.minidom
import shutil
import os
import matplotlib.pyplot as plt
from matplotlib import style

def evaluate_palm():
	global p_tp,p_fp,p_tn,p_fn
	global f_tp,f_fp,f_tn,f_fn
	global total_iou, detect_num
	global t_tp,t_fp,t_tn,t_fn
	for i in range(palm_img_num):
		ipath = 'test_dataset/palm/img/palm_test'+str(i+1)+imgsuffix
		xpath = 'test_dataset/palm/annotation/palm_test'+str(i+1)+xmlsuffix
		# print(ipath,xpath)

		x1i,y1i,x2i,y2i = 0,0,0,0
		iou = 0.0

		try:
			dom = xml.dom.minidom.parse(xpath)
			root = dom.documentElement
			typeroot = root.getElementsByTagName('name')
			# for rootnum in range(len(typeroot)):
			typenum = typeroot[0].firstChild.data
			if typenum == "palm":
				x1i = int(root.getElementsByTagName('xmin')[0].firstChild.data)
				y1i = int(root.getElementsByTagName('ymin')[0].firstChild.data)
				x2i = int(root.getElementsByTagName('xmax')[0].firstChild.data)
				y2i = int(root.getElementsByTagName('ymax')[0].firstChild.data)
		
		except FileNotFoundError:
			continue

		print('evaluating %s'% ipath)
		img = cv2.imread(ipath)
		palms = palm_cascade.detectMultiScale(img, 1.1, detect_neighbor,0|cv2.CASCADE_SCALE_IMAGE,minSize=(20,20))
		# for palm in palms:
		# 	print(palm)
		if len(palms)==0:
			iou = 0.0
		if len(palms)>=1:
			detect_num += 1
			for palm in palms:
				x1j,y1j,width,height = palm
				x2j = x1j + width
				y2j = y1j + height
				areai = (x2i-x1i+1)*(y2i-y1i+1)
				areaj = (x2j-x1j+1)*(y2j-y1j+1)

				xx1 = max(x1i, x1j)
				yy1 = max(y1i, y1j)
				xx2 = min(x2i, x2j)
				yy2 = min(y2i, y2j)

				h = max(0, yy2-yy1+1)
				w = max(0, xx2-xx1+1)

				intersection = w * h
				iou = max(iou,float(intersection) / float(areai + areaj - intersection))

		total_iou += iou
		print ("IoU is %.4f" % iou)
		if iou>=iou_border:
			p_tp+=1
		if iou>0 and iou<iou_border:
			p_fp+=1
		if iou==0 and len(palms)>=1:
			p_fp+=1
		if len(palms)==0:
			p_fn+=1

		fists = fist_cascade.detectMultiScale(img, 1.1, detect_neighbor,0|cv2.CASCADE_SCALE_IMAGE,minSize=(20,20))
		if len(fists)>=1:
			f_fp+=1
		else:
			f_tn+=1

def evaluate_fist():
	global f_tp,f_fp,f_tn,f_fn
	global p_tp,p_fp,p_tn,p_fn
	global total_iou, detect_num
	global t_tp,t_fp,t_tn,t_fn
	for i in range(fist_img_num):
		ipath = 'test_dataset/fist/img/fist_test'+str(i+1)+imgsuffix
		xpath = 'test_dataset/fist/annotation/fist_test'+str(i+1)+xmlsuffix
		# print(ipath,xpath)

		x1i,y1i,x2i,y2i = 0,0,0,0
		iou = 0.0

		try:
			dom = xml.dom.minidom.parse(xpath)
			root = dom.documentElement
			typeroot = root.getElementsByTagName('name')
			# for rootnum in range(len(typeroot)):
			typenum = typeroot[0].firstChild.data
			if typenum == "fist":
				x1i = int(root.getElementsByTagName('xmin')[0].firstChild.data)
				y1i = int(root.getElementsByTagName('ymin')[0].firstChild.data)
				x2i = int(root.getElementsByTagName('xmax')[0].firstChild.data)
				y2i = int(root.getElementsByTagName('ymax')[0].firstChild.data)

		except FileNotFoundError:
			continue

		print('evaluating %s'% ipath)
		img = cv2.imread(ipath)
		fists = fist_cascade.detectMultiScale(img, 1.1, detect_neighbor,0|cv2.CASCADE_SCALE_IMAGE,minSize=(20,20))
		# for palm in palms:
		# 	print(palm)
		if len(fists)==0:
			iou = 0.0
		if len(fists)>=1:
			detect_num += 1
			for fist in fists:
				x1j,y1j,width,height = fist
				x2j = x1j + width
				y2j = y1j + height
				areai = (x2i-x1i+1)*(y2i-y1i+1)
				areaj = (x2j-x1j+1)*(y2j-y1j+1)

				xx1 = max(x1i, x1j)
				yy1 = max(y1i, y1j)
				xx2 = min(x2i, x2j)
				yy2 = min(y2i, y2j)

				h = max(0, yy2-yy1+1)
				w = max(0, xx2-xx1+1)

				intersection = w * h
				iou = max(iou,float(intersection) / float(areai + areaj - intersection))

		total_iou += iou
		print ("IoU is %.4f" % iou)
		if iou>=iou_border:
			f_tp+=1
		if iou>0 and iou<iou_border:
			f_fp+=1
		if iou==0 and len(fists)>=1:
			f_fp+=1
		if len(fists)==0:
			f_fn+=1

		palms = palm_cascade.detectMultiScale(img, 1.1, detect_neighbor,0|cv2.CASCADE_SCALE_IMAGE,minSize=(20,20))
		if len(palms)>=1:
			p_fp+=1
		else:
			p_tn+=1

imgsuffix = '.jpg'
xmlsuffix = '.xml'
palm_cascade = cv2.CascadeClassifier('palm_v4.xml')
fist_cascade = cv2.CascadeClassifier('fist_v3.xml')

palm_img_num = sum([len(x) for _, _, x in os.walk(os.path.dirname("test_dataset/palm/img/"))])
fist_img_num = sum([len(x) for _, _, x in os.walk(os.path.dirname("test_dataset/fist/img/"))])
print("%d palm images, %d fist images detected for testing"%(palm_img_num,fist_img_num))

tp_list = [1.0]
fp_list = [1.0]
pr_list = [0.0]
f_tp,f_fp,f_tn,f_fn = 0.0,0.0,0.0,0.0
p_tp,p_fp,p_tn,p_fn = 0.0,0.0,0.0,0.0
t_tp,t_fp,t_tn,t_fn = 0.0,0.0,0.0,0.0
detect_neighbor = 1
iou_border = 0.5
total_iou = 0.0
detect_num = 0

for i in range(20):
	f_tp,f_fp,f_tn,f_fn = 0.0,0.0,0.0,0.0
	p_tp,p_fp,p_tn,p_fn = 0.0,0.0,0.0,0.0
	detect_neighbor = 1+i
	print("Setting minimum detect neighbor= %d"%detect_neighbor)
	evaluate_fist()
	evaluate_palm()

	print("Palm: Precision:%.2f, Recall:%.2f, Accuracy:%.2f"%(p_tp/(p_tp+p_fp), \
		p_tp/(p_tp+p_fn),(p_tp+p_tn)/(p_tn+p_tp+p_fn+p_fp)))
	print("Fist: Precision:%.2f, Recall:%.2f, Accuracy:%.2f"%(f_tp/(f_tp+f_fp), \
		f_tp/(f_tp+f_fn),(f_tp+f_tn)/(f_tn+f_tp+f_fn+f_fp)))

	t_tp += f_tp+p_tp
	t_fp += f_fp+p_fp
	t_tn += f_tn+p_tn
	t_fn += f_fn+p_fn

	fp_list.append(float('%.4f'%((p_fp+f_fp)/(p_fp+p_tn+f_fp+f_tn))))
	tp_list.append(float('%.4f'%((p_tp+f_tp)/(p_tp+p_fn+f_tp+f_fn))))
	pr_list.append(float('%.4f'%((p_tp+f_tp)/(p_tp+p_fp+f_tp+f_fp))))

print("Average IoU: %.2f, Average Precision: %.2f, Average Recall: %.2f, Average Accuracy: %.2f" \
	%(total_iou/detect_num,t_tp/(t_tp+t_fp),t_tp/(t_tp+t_fn),(t_tp+t_tn)/(t_tn+t_tp+t_fn+t_fp)))

fp_list.append(0.0)
tp_list.append(0.0)
pr_list.append(1.0)
style.use('ggplot')
fig = plt.figure(1)
ax1 = fig.add_subplot(121)
plt.xlabel('False positive rate')
plt.ylabel('True positive rate')
plt.title('Comparison in ROC space')
plt.grid(True)
plt.legend()
ax1.plot(fp_list,tp_list)

ax2 = fig.add_subplot(122)
plt.xlabel('Recall')
plt.ylabel('Precision')
plt.title('Comparison in PR space')
plt.grid(True)
plt.legend()
ax2.plot(tp_list,pr_list)
plt.show()
# print(p_fp/(p_fp+p_tn),f_fp/(f_fp+f_tn))
