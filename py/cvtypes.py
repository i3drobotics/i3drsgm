import numpy as np

CV_8UC1,CV_8UC2,CV_8UC3,CV_8UC4 = 0,8,16,24
CV_8SC1,CV_8SC2,CV_8SC3,CV_8SC4 = 1,9,17,25
CV_16UC1,CV_16UC2,CV_16UC3,CV_16UC4 = 2,10,18,26
CV_16SC1,CV_16SC2,CV_16SC3,CV_16SC4 = 3,11,19,27
CV_32SC1,CV_32SC2,CV_32SC3,CV_32SC4 = 4,12,20,28
CV_32FC1,CV_32FC2,CV_32FC3,CV_32FC4 = 5,13,21,29
CV_64FC1,CV_64FC2,CV_64FC3,CV_64FC4 = 6,14,22,30

def dtype_to_cvtype(dtype,channels):
    cvtype = None
    if dtype == np.uint8:
        if channels == 1:
            cvtype = CV_8UC1
        elif channels == 2:
            cvtype = CV_8UC2
        elif channels == 3:
            cvtype = CV_8UC3
        elif channels == 3:
            cvtype = CV_8UC4
    if dtype == np.int8:
        if channels == 1:
            cvtype = CV_8SC1
        elif channels == 2:
            cvtype = CV_8SC2
        elif channels == 3:
            cvtype = CV_8SC3
        elif channels == 3:
            cvtype = CV_8SC4
    if dtype == np.ushort:
        if channels == 1:
            cvtype = CV_16UC1
        elif channels == 2:
            cvtype = CV_16UC2
        elif channels == 3:
            cvtype = CV_16UC3
        elif channels == 3:
            cvtype = CV_16UC4
    if dtype == np.short:
        if channels == 1:
            cvtype = CV_16SC1
        elif channels == 2:
            cvtype = CV_16SC2
        elif channels == 3:
            cvtype = CV_16SC3
        elif channels == 3:
            cvtype = CV_16SC4
    if dtype == np.int32:
        if channels == 1:
            cvtype = CV_32SC1
        elif channels == 2:
            cvtype = CV_32SC2
        elif channels == 3:
            cvtype = CV_32SC3
        elif channels == 3:
            cvtype = CV_32SC4
    if dtype == np.float32:
        if channels == 1:
            cvtype = CV_32FC1
        elif channels == 2:
            cvtype = CV_32FC2
        elif channels == 3:
            cvtype = CV_32FC3
        elif channels == 3:
            cvtype = CV_32FC4
    if dtype == np.float64:
        if channels == 1:
            cvtype = CV_64FC1
        elif channels == 2:
            cvtype = CV_64FC2
        elif channels == 3:
            cvtype = CV_64FC3
        elif channels == 3:
            cvtype = CV_64FC4
    return cvtype
            