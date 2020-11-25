import cv2
import numpy as np
import subprocess
import os
from Stereo3D import Stereo3D, StereoCalibration
from Stereo3D.StereoCapture import *

class I3DRSGMAppAPI:
    def __init__(self, output_folder=None, I3RSGMApp_folder=None):
        script_folder = os.path.dirname(os.path.realpath(__file__))
        self.init_success = False
        self.PARAM_MIN_DISPARITY = "SET_MIN_DISPARITY"
        self.PARAM_DISPARITY_RANGE = "SET_DISPARITY_RANGE"

        if (I3RSGMApp_folder == None):
            self.I3DRSGMApp = os.path.join(script_folder,'../I3DRSGMApp.exe')
        else:
            self.I3DRSGMApp = os.path.join(I3RSGMApp_folder,"I3DRSGMApp.exe")

        if (output_folder == None):
            script_folder = os.path.dirname(os.path.realpath(__file__))
            output_folder = os.path.join(script_folder,'tmp')
        if not os.path.exists(output_folder):
                os.makedirs(os.path.join(output_folder))
        self.output_folder = output_folder

        # Start I3DRSGMApp with API argument
        self.appProcess = subprocess.Popen([self.I3DRSGMApp, "api"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        self.init_success = True
        # Send initalisation request to I3DRSGM API
        valid,response = self.apiRequest("INIT")
        if (not valid):
            print("Failed to initalise I3DRSGM: "+response)
            self.close()
        self.init_success = valid

    def isInit(self):
        return self.init_success

    def removePrefix(self,text, prefix):
        if text.startswith(prefix):
            return text[len(prefix):]
        return text  # or whatever

    def apiWaitResponse(self):
        if (self.init_success):
            while(True):
                line = self.appProcess.stdout.readline()
                line_str = line.decode("utf-8")
                if (line_str  == "API_READY\r\n"):
                    return True,line_str
                elif (line_str.startswith("API_RESPONSE:")):
                    response = self.removePrefix(line_str,"API_RESPONSE:")
                    if (response.startswith("ERROR,")):
                        error_msg = self.removePrefix(response,"ERROR,")
                        return False,error_msg.rstrip()
                    else:
                        return True,response.rstrip()
                elif (line_str == ""):
                    return False,line_str
                else:
                    pass
                    #print("stout:"+line_str)
        else:
            print("Failed to initalise the pyI3DRSGM class. Make sure to initalise the class 'i3rsgm = pyI3DRSGM(...'")
            print("Check valid initalisation with 'isInit' function. E.g. 'i3rsgm.isInit()'")

    def apiRequest(self,cmd):
        if (self.init_success):
            valid,response = self.apiWaitResponse()
            if (valid):
                print("sending api request...")
                self.appProcess.stdin.write((cmd+"\n").encode())
                self.appProcess.stdin.flush()
                print("waiting for api response...")
                valid,response = self.apiWaitResponse()
            return valid,response
        else:
            print("Failed to initalise the pyI3DRSGM class. Make sure to initalise the class 'i3rsgm = pyI3DRSGM(...'")
            print("Check valid initalisation with 'isInit' function. E.g. 'i3rsgm.isInit()'")
            return False,"" 

    def forwardMatchFiles(self, left_filepath, right_filepath, left_cal_filepath=None, right_cal_filepath=None):
        if (self.init_success):
            if (left_cal_filepath == None or right_cal_filepath == None):
                appOptions="FORWARD_MATCH,"+left_filepath+","+right_filepath+","+self.output_folder+",0"
            else:
                appOptions="FORWARD_MATCH,"+left_filepath+","+right_filepath+","+left_cal_filepath+","+right_cal_filepath+","+self.output_folder+",0"
            valid,response = self.apiRequest(appOptions)
            print(response)
            return valid
        else:
            print("Failed to initalise the pyI3DRSGM class. Make sure to initalise the class 'i3rsgm = pyI3DRSGM(...'")
            print("Check valid initalisation with 'isInit' function. E.g. 'i3rsgm.isInit()'")
            return valid
    def setParam(self,param,value):
        if (self.init_success):
            appOptions=param+","+value
            valid,_ = self.apiRequest(appOptions)
            return valid
        else:
            print("Failed to initalise the pyI3DRSGM class. Make sure to initalise the class 'i3rsgm = pyI3DRSGM(...'")
            print("Check valid initalisation with 'isInit' function. E.g. 'i3rsgm.isInit()'")
            return False

    def close(self):
        self.appProcess.terminate()

class pyI3DRSGM:

    def __init__(self, tmp_folder=None, I3RSGMApp_folder=None):
        self.i3drsgmAppAPI = I3DRSGMAppAPI(tmp_folder, I3RSGMApp_folder)

    def isInit(self):
        return self.i3drsgmAppAPI.isInit()

    def forwardMatch(self, left_img, right_img):
        if (self.isInit()):
            left_filepath=os.path.join(self.i3drsgmAppAPI.output_folder,"left_tmp.png")
            right_filepath=os.path.join(self.i3drsgmAppAPI.output_folder,"right_tmp.png")
            disp_filepath=os.path.join(self.i3drsgmAppAPI.output_folder,"disparity.tif")
            cv2.imwrite(left_filepath,left_img)
            cv2.imwrite(right_filepath,right_img)
            valid = self.i3drsgmAppAPI.forwardMatchFiles(left_filepath,right_filepath)
            disp = None
            if (valid):
                disp = cv2.imread(disp_filepath,-1)
            return valid,disp
        else:
            print("Failed to initalise the pyI3DRSGM class. Make sure to initalise the class 'i3rsgm = pyI3DRSGM(...'")
            print("Check valid initalisation with 'isInit' function. E.g. 'i3rsgm.isInit()'")
            return valid,None

    def setDisparityRange(self,value):
        if (self.isInit()):
            valid = self.i3drsgmAppAPI.setParam(self.i3drsgmAppAPI.PARAM_DISPARITY_RANGE,value)
            return valid
        else:
            print("Failed to initalise the pyI3DRSGM class. Make sure to initalise the class 'i3rsgm = pyI3DRSGM(...'")
            print("Check valid initalisation with 'isInit' function. E.g. 'i3rsgm.isInit()'")
            return False

    def setMinDisparity(self,value):
        if (self.isInit()):
            valid = self.i3drsgmAppAPI.setParam(self.i3drsgmAppAPI.PARAM_MIN_DISPARITY,value)
            return valid
        else:
            print("Failed to initalise the pyI3DRSGM class. Make sure to initalise the class 'i3rsgm = pyI3DRSGM(...'")
            print("Check valid initalisation with 'isInit' function. E.g. 'i3rsgm.isInit()'")
            return False

    def close(self):
        self.i3drsgmAppAPI.close()

if __name__ == '__main__':
    script_folder = os.path.dirname(os.path.realpath(__file__))
    resource_folder = os.path.join(script_folder,"../resources")

    # define calibration files for left and right image
    left_cal_file = os.path.join(resource_folder,"left.yaml")
    right_cal_file = os.path.join(resource_folder,"right.yaml")
    # get calibration from yaml files
    stcal = StereoCalibration()
    stcal.get_cal_from_yaml(left_cal_file,right_cal_file)

    i3drsgm = pyI3DRSGM(resource_folder)
    if (i3drsgm.isInit()):
        valid = True
        left_img = cv2.imread(os.path.join(resource_folder,"left.png"))
        right_img = cv2.imread(os.path.join(resource_folder,"right.png"))
        while(valid):
            # valid = i3drsgm.forwardMatchFiles(
            #     os.path.join(resource_folder,"/left.png"),
            #     os.path.join(resource_folder,"/right.png"))
            left_rect_img, right_rect_img = stcal.rectify_pair(left_img,right_img)
            valid,disp = i3drsgm.forwardMatch(left_rect_img,right_rect_img)
        i3drsgm.close()

'''
import numpy as np
import ctypes as C
import cvtypes
from ctypes import cdll, c_void_p, c_long
import cv2

#TODO: Fix issue of missing dlls when importing I3DRSGM.dll
print("Loading I3DRSGM library...")
libI3DRSGM = C.cdll.LoadLibrary('C:/Code/I3DR/i3drsgm/install/i3drsgm/i3drsgm-1.0.6/bin/libI3DRSGM.dll')
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
'''