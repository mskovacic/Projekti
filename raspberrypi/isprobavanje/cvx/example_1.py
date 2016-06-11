#Total variation in-painting
#Grayscale Images
#http://nbviewer.jupyter.org/github/cvxgrp/cvxpy/blob/master/examples/notebooks/WWW/tv_inpainting.ipynb

from PIL import Image
#import matplotlib.pyplot as plt
import numpy as np
from cvxpy import *

def get_greyscale((r,g,b,a)):
	return (r+g+b)/3

# Load the images.
orig_img = Image.open("lena.png")
corr_img = Image.open("lena_corrupted.png")

# Convert to arrays.
Uorig = np.array(orig_img)
Ucorr = np.array(corr_img)
rows, cols, tmp= Uorig.shape

# Known is 1 if the pixel is known,
# 0 if the pixel was corrupted.
Known = np.zeros((rows, cols))
Ucorrupted = []
Uoriginal = []
for i in xrange(rows):
	row_corrupted = []
	row_original = []
	for j in xrange(cols):
		#print Uorig[i, j], Ucorr[i, j]
		if Uorig[i, j].all() == Ucorr[i, j].all():
			Known[i, j] = 1
		row_corrupted.append(get_greyscale(Ucorr[i, j]))
		row_original.append(get_greyscale(Uorig[i, j]))
	Ucorrupted.append(row_corrupted)
	Uoriginal.append(row_original)

#%matplotlib inline
#fig, ax = plt.subplots(1, 2,figsize=(10, 5))
#ax[0].imshow(orig_img);
#ax[0].set_title("Original Image")
#ax[0].axis('off')
#ax[1].imshow(corr_img);
#ax[1].set_title("Corrupted Image")
#ax[1].axis('off');

# Recover the original image using total variation in-painting.
U = Variable(rows, cols, 4)
obj = Minimize(tv(U))
constraints = [mul_elemwise(Known, U) == mul_elemwise(Known, Ucorr)]
prob = Problem(obj, constraints)
# Use SCS to solve the problem.
prob.solve(verbose=True, solver=SCS)

#fig, ax = plt.subplots(1, 2,figsize=(10, 5))
# Display the in-painted image.
img_rec = Image.fromarray(U.value)
#ax[0].imshow(img_rec);
#ax[0].set_title("In-Painted Image")
#ax[0].axis('off')

img_diff = Image.fromarray(10*np.abs(Uoriginal - U.value))
#ax[1].imshow(img_diff);
#ax[1].set_title("Difference Image")
#ax[1].axis('off');

box = orig_img.getbbox()
image = Image.new("RGB", (box[2], box[3]))
image.paste(img_rec, (0,0))
image.save("lena_recovered.png")

image = Image.new("RGB", (box[2], box[3]))
image.paste(img_diff, (0,0))
image.save("lena_d.png")