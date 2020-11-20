import cv2
import numpy as np
import subprocess
import os

class pyI3DRSGM:
    def __init__(self,left_cal_filepath, right_cal_filepath, output_folder):
        self.left_cal_filepath, self.right_cal_filepath, self.output_folder = left_cal_filepath, right_cal_filepath, output_folder
        self.script_folder = os.path.dirname(os.path.realpath(__file__))
        self.I3DRSGMApp = script_folder+'/../I3DRSGMApp.exe'
        cmd = [self.I3DRSGMApp, "api"]
        self.appProcess = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        self.apiRequest("INIT")

    def remove_prefix(self,text, prefix):
        if text.startswith(prefix):
            return text[len(prefix):]
        return text  # or whatever

    def apiWaitResponse(self):
        while(True):
            line = self.appProcess.stdout.readline()
            line_str = line.decode("utf-8")
            if (line_str  == "API_READY\r\n"):
                return True,line_str
            elif (line_str.startswith("API_RESPONSE:")):
                response = self.remove_prefix(line_str,"API_RESPONSE:")
                if (response.startswith("ERROR,")):
                    error_msg = self.remove_prefix(response,"API_RESPONSE:")
                    return False,error_msg
                else:
                    return True,response
            elif (line_str == ""):
                return False,line_str
            else:
                print("stout:"+line_str)

    def apiRequest(self,cmd):
        valid,response = self.apiWaitResponse()
        if (valid):
            print("sending api request...")
            self.appProcess.stdin.write((cmd+"\n").encode())
            self.appProcess.stdin.flush()
            print("waiting for api response...")
            valid,response = self.apiWaitResponse()
        return valid,response

    def forwardMatchFiles(self, left_filepath, right_filepath):
        #cmd = [self.I3DRSGMApp, left_filepath, right_filepath, left_cal_filepath, right_cal_filepath, output_folder, "0", "1"]
        appOptions="FORWARD_MATCH,"+left_filepath+","+right_filepath+","+self.left_cal_filepath+","+self.right_cal_filepath+","+self.output_folder+",0,1"
        valid,response = self.apiRequest(appOptions)
        print(response)
        return valid

    def forwardMatch(self, left_img, right_img):
        if not os.path.exists(self.script_folder+"/../tmp/"):
            os.makedirs(self.script_folder+"/../tmp/")
        left_filepath=self.script_folder+"/../tmp/left_tmp.png"
        right_filepath=self.script_folder+"/../tmp/right_tmp.png"
        disp_filepath=self.output_folder+"/disparity.tif"
        cv2.imwrite(left_filepath,left_img)
        cv2.imwrite(right_filepath,right_img)
        valid = self.forwardMatchFiles(left_filepath,right_filepath)
        disp = None
        if (valid):
            disp = cv2.imread(disp_filepath,-1)
        return valid,disp

    def close(self):
        self.appProcess.terminate()

if __name__ == '__main__':
    script_folder = os.path.dirname(os.path.realpath(__file__))
    resource_folder = script_folder+"/../resources"
    i3drsgm = pyI3DRSGM(resource_folder+"/left.yaml",resource_folder+"/right.yaml",resource_folder+"/output")
    valid = True
    left_img = cv2.imread(resource_folder+"/left.png")
    right_img = cv2.imread(resource_folder+"/right.png")
    while(valid):
        #valid = i3drsgm.forwardMatchFiles(resource_folder+"/left.png",resource_folder+"/right.png")
        valid,disp = i3drsgm.forwardMatch(left_img,right_img)
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