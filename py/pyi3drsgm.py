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
        self.PARAM_INTERPOLATION = "SET_INTERPOLATION"

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
                #print("sending api request...")
                self.appProcess.stdin.write((cmd+"\n").encode())
                self.appProcess.stdin.flush()
                #print("waiting for api response...")
                valid,response = self.apiWaitResponse()
            return valid,response
        else:
            print("Failed to initalise the pyI3DRSGM class. Make sure to initalise the class 'i3rsgm = pyI3DRSGM(...'")
            print("Check valid initalisation with 'isInit' function. E.g. 'i3rsgm.isInit()'")
            return False,"" 

    def forwardMatchFiles(self, left_filepath, right_filepath, left_cal_filepath=None, right_cal_filepath=None):
        if (self.init_success):
            if (left_cal_filepath == None or right_cal_filepath == None):
                appOptions="FORWARD_MATCH,"+left_filepath+","+right_filepath+","+self.output_folder
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
            appOptions=param+","+str(value)
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

    def enableInterpolation(self,enable):
        if (self.isInit()):
            if (enable):
                val = 1
            else:
                val = 0
            valid = self.i3drsgmAppAPI.setParam(self.i3drsgmAppAPI.PARAM_INTERPOLATION,val)
            return valid
        else:
            print("Failed to initalise the pyI3DRSGM class. Make sure to initalise the class 'i3rsgm = pyI3DRSGM(...'")
            print("Check valid initalisation with 'isInit' function. E.g. 'i3rsgm.isInit()'")
            return False

    def close(self):
        self.i3drsgmAppAPI.close()

def scale_disparity(disparity):
    minV, maxV,_,_ = cv2.minMaxLoc(disparity)
    if (maxV - minV != 0):
        scaled_disp = cv2.convertScaleAbs(disparity, alpha=255.0/(maxV - minV), beta=-minV * 255.0/(maxV - minV))
        return scaled_disp
    else:
        return np.zeros(disparity.shape, np.uint8)

def image_resize(image, width = None, height = None, inter = cv2.INTER_AREA):
    """
    Resize image based on height or width while maintaning aspect ratio
    :param image: image matrix
    :param width: desired width of output image (can only use width or height not both)
    :param height: desired height of output image (can only use width or height not both)
    :param inter: opencv resize method (default: cv2.INTER_AREA)
    :type image: numpy
    :type width: int
    :type height: int
    :type inter: int
    """
    # initialize the dimensions of the image to be resized and
    # grab the image size
    dim = None
    (h, w) = image.shape[:2]

    # if both the width and height are None, then return the
    # original image
    if width is None and height is None:
        return image

    # check to see if the width is None
    if width is None:
        # calculate the ratio of the height and construct the
        # dimensions
        r = height / float(h)
        dim = (int(w * r), height)

    # otherwise, the height is None
    else:
        # calculate the ratio of the width and construct the
        # dimensions
        r = width / float(w)
        dim = (width, int(h * r))

    # resize the image
    resized = cv2.resize(image, dim, interpolation = inter)

    # return the resized image
    return resized

def display_disp(disp,Q):
    wz = Q[2, 3]
    q03 = Q[0, 3]
    q13 = Q[1, 3]
    q32 = Q[3, 2]
    q33 = Q[3, 3]

    print("Filtering disparity...")
    disparity16 = -disp.astype(np.float32)

    w = (disparity16 * q32) + q33

    minW = np.min(w)
    maxW = np.max(w)

    w_zero_mask = w <= 0
    disparity16[w_zero_mask != 0] = 0.0

    d_inf_mask = disparity16 == 99999
    disparity16[d_inf_mask != 0] = 0.0

    masked_a = np.ma.masked_equal(disparity16, 0.0, copy=False)
    minDisp = masked_a.min()
    maxDisp = masked_a.max()

    disparity16[w_zero_mask != 0] = minDisp
    disparity16[d_inf_mask != 0] = maxDisp
    disp_scaled = scale_disparity(disparity16)

    print("Generating depth from disparity...")
    depth = cv2.reprojectImageTo3D(disparity16, Q)
    
    w_zero_mask = w <= 0
    depth[w_zero_mask != 0] = [ 0, 0, 0]

    x,y,z = cv2.split(depth)
    z[w_zero_mask != 0] = 0.0
    z[d_inf_mask != 0] = 0.0
    depth = cv2.merge((x,y,z))

    masked_depth = np.ma.masked_equal(z, 0.0, copy=False)
    minDepth = masked_depth.min()
    maxDepth = masked_depth.max()
    print("Depth range: "+str(minDepth)+","+str(maxDepth))

    print("Applying colormap to disparity...")

    # apply color map to disparity
    disp_colormap = cv2.applyColorMap(disp_scaled, cv2.COLORMAP_JET)
    disp_colormap[w_zero_mask != 0] = [0, 0, 0]
    disp_colormap[d_inf_mask != 0] = [0, 0, 0]

    disp_colormap_resized = image_resize(disp_colormap, height=640)
    cv2.imshow("display", disp_colormap_resized)
    cv2.waitKey(1)


if __name__ == '__main__':
    script_folder = os.path.dirname(os.path.realpath(__file__))
    resource_folder = os.path.join(script_folder,"../resources")

    # load stereo calibration from yamls
    left_cal_file = os.path.join(resource_folder,"sim_left.yaml")
    right_cal_file = os.path.join(resource_folder,"sim_right.yaml")
    stcal = StereoCalibration()
    stcal.get_cal_from_yaml(left_cal_file,right_cal_file)

    #initalise I3DRSGM
    print("Intitalising I3DRSGM...")
    i3drsgm = pyI3DRSGM(resource_folder)
    if (i3drsgm.isInit()):
        # load images from file
        left_img = cv2.imread(os.path.join(resource_folder,"sim_left.png"))
        right_img = cv2.imread(os.path.join(resource_folder,"sim_right.png"))
        left_gray_img = cv2.cvtColor(left_img, cv2.COLOR_BGR2GRAY)
        right_gray_img = cv2.cvtColor(right_img, cv2.COLOR_BGR2GRAY)

        # Get Q from calibration
        Q = stcal.stereo_cal["q"]

        # Set matcher parameters
        i3drsgm.setDisparityRange(0)
        i3drsgm.setDisparityRange(3264)
        i3drsgm.enableInterpolation(False)

        valid = True
        while(valid):
            # Rectify stereo image pair
            left_rect_img, right_rect_img = stcal.rectify_pair(left_gray_img,right_gray_img)
            # Stereo match image pair
            print("Running I3DRSGM on images...")
            valid,disp = i3drsgm.forwardMatch(left_rect_img,right_rect_img)
            if (valid):
                # Display normalised disparity in OpenCV window
                display_disp(disp,Q)

        # Important to close I3DRSGM to clean up memory
        i3drsgm.close()