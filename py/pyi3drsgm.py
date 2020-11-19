import numpy as np
import ctypes as C
import cvtypes
from ctypes import cdll, c_void_p, c_long
import cv2

import os
os.environ['PATH'] = 'C:/Code/I3DR/i3drsgm/install/i3drsgm/i3drsgm-1.0.6/bin/' + os.environ['PATH']

print("Loading I3DRSGM library...")
libI3DRSGM = C.cdll.LoadLibrary('C:/Code/I3DR/i3drsgm/install/i3drsgm/i3drsgm-1.0.6/bin/I3DRSGM.dll')
print("I3DRSGM library load complete")

#def TestLicense():
#    return libI3DRSGM.TestLicense()

def showimg(img):
    # Cast to c type pointer and 2 longs W/H.
    rows = c_long(img.shape[0])
    cols = c_long(img.shape[1])
    cvtype = cvtypes.dtype_to_cvtype(img.dtype,img.shape[2])
    data = img.ctypes.data_as(c_void_p)  # The C pointer
    
    libI3DRSGM.showimg(rows, cols, cvtype, data)
    return

def cvinout(img):
    # Cast input image to c type pointer and 2 longs W/H.
    rows_in = c_long(img.shape[0])
    cols_in = c_long(img.shape[1])
    cvtype_in = cvtypes.dtype_to_cvtype(img.dtype,img.shape[2])
    data_in = img.ctypes.data_as(c_void_p)  # The C pointer
    
    # create numpy array for output to be filled into
    # MUST be created before calling the function so the memory is assigned
    img_out = np.zeros(dtype=np.uint8, shape=(img.shape[0], img.shape[1], img.shape[2]))
    rows_out = c_long(img_out.shape[0])
    cols_out = c_long(img_out.shape[1])
    cvtype_out = cvtypes.dtype_to_cvtype(img_out.dtype,img_out.shape[2])
    data_out = img_out.ctypes.data_as(c_void_p)  # The C pointer

    libI3DRSGM.cvinout(
        rows_in, cols_in, cvtype_in, data_in,
        rows_out, cols_out, cvtype_out, data_out
    )
    return img_out

if __name__ == '__main__':
    if True:
        print("I3DRSGM license valid")

        c = cv2.VideoCapture(0)
        while True:
            _, f = c.read()
            cv2.imshow('f', f)
            cv2.imshow('cvinout', cvinout(f))
            if cv2.waitKey(1) == 27:
                break
    else:
        print("I3DRSGM license invalid")
    