import os
import subprocess
import math
import shutil
import numpy as np
import scipy
import cv2
from Stereo3D import StereoCalibration
from stereosupport import colormap_from_disparity, depth_from_disp, image_resize

class I3DRSGMAppAPI:
    def __init__(self, output_folder=None, I3RSGMApp_folder=None, license_file=None):
        # Initialise I3DRSGM App API
        # Get folder containing current script
        script_folder = os.path.dirname(os.path.realpath(__file__))
        # Init variables
        self.init_success = False
        self.PARAM_MIN_DISPARITY = "SET_MIN_DISPARITY"
        self.PARAM_DISPARITY_RANGE = "SET_DISPARITY_RANGE"
        self.PARAM_INTERPOLATION = "SET_INTERPOLATION"

        # Define location of I3DRSGM app
        if (I3RSGMApp_folder == None):
            self.I3DRSGMApp = os.path.join(script_folder,'../I3DRSGMApp.exe')
        else:
            self.I3DRSGMApp = os.path.join(I3RSGMApp_folder,"I3DRSGMApp.exe")

        # Define output folder used for storing images while processing
        if (output_folder == None):
            script_folder = os.path.dirname(os.path.realpath(__file__))
            output_folder = os.path.join(script_folder,'tmp')
        if not os.path.exists(output_folder):
                os.makedirs(os.path.join(output_folder))
        self.output_folder = output_folder

        # Copy license file to I3DRSGM path
        if license_file is not None:
            if os.path.exists(license_file):
                if os.path.isfile(license_file):
                    shutil.copy2(license_file,os.path.dirname(self.I3DRSGMApp))
                else:
                    print("license_file parameter expects a file")
                    self.init_success = False
                    return
            else:
                print("license file does not exist")
                self.init_success = False
                return

        # Start I3DRSGMApp with API argument
        self.appProcess = subprocess.Popen([self.I3DRSGMApp, "api"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        self.init_success = True
        # Send initalisation request to I3DRSGM API
        valid,response = self.apiRequest("INIT")
        if (not valid): # Check init request was successful
            print("Failed to initalise I3DRSGM: "+response)
            self.close()
        self.init_success = valid

    def isInit(self):
        # Check if class was initalised successfully
        return self.init_success

    def removePrefix(self,text, prefix):
        # Remove prefix from string
        if text.startswith(prefix):
            return text[len(prefix):]
        return text  # or whatever

    def apiWaitResponse(self):
        # Wait for reponse from I3DRSGM app API.
        # This is important to keep the pipe buffer clean
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
        # Perform an API requst with the I3DRSGM app
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
        # Stereo match from left and right image filepaths
        if (self.init_success):
            if (left_cal_filepath == None or right_cal_filepath == None):
                appOptions="FORWARD_MATCH,"+left_filepath+","+right_filepath+","+self.output_folder
            else:
                appOptions="FORWARD_MATCH,"+left_filepath+","+right_filepath+","+left_cal_filepath+","+right_cal_filepath+","+self.output_folder+",0"
            valid,response = self.apiRequest(appOptions)
            if (not valid):
                print(response)
            return valid
        else:
            print("Failed to initalise the pyI3DRSGM class. Make sure to initalise the class 'i3rsgm = pyI3DRSGM(...'")
            print("Check valid initalisation with 'isInit' function. E.g. 'i3rsgm.isInit()'")
            return valid

    def setParam(self,param,value):
        # Set algorithm parameter with api request
        if (self.init_success):
            appOptions=param+","+str(value)
            valid,_ = self.apiRequest(appOptions)
            return valid
        else:
            print("Failed to initalise the pyI3DRSGM class. Make sure to initalise the class 'i3rsgm = pyI3DRSGM(...'")
            print("Check valid initalisation with 'isInit' function. E.g. 'i3rsgm.isInit()'")
            return False

    def close(self):
        # Close connection to app process
        # Required to clean up memory
        self.appProcess.terminate()

class pyI3DRSGM:

    def __init__(self, tmp_folder=None, I3RSGMApp_folder=None, license_file=None):
        # Initalse I3DRSGM
        # Initalise connection to I3DRSGM app API
        self.i3drsgmAppAPI = I3DRSGMAppAPI(tmp_folder, I3RSGMApp_folder, license_file)

    def isInit(self):
        # Check I3DRSGM has been initalised
        return self.i3drsgmAppAPI.isInit()

    def forwardMatch(self, left_img, right_img):
        # Stereo matching using a left and right image (expects images to already by rectified)
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
        # Set disparity range used I3DRSGM algorithm
        if (self.isInit()):
            valid = self.i3drsgmAppAPI.setParam(self.i3drsgmAppAPI.PARAM_DISPARITY_RANGE,value)
            return valid
        else:
            print("Failed to initalise the pyI3DRSGM class. Make sure to initalise the class 'i3rsgm = pyI3DRSGM(...'")
            print("Check valid initalisation with 'isInit' function. E.g. 'i3rsgm.isInit()'")
            return False

    def setMinDisparity(self,value):
        # Set minimum disparity used I3DRSGM algorithm
        if (self.isInit()):
            valid = self.i3drsgmAppAPI.setParam(self.i3drsgmAppAPI.PARAM_MIN_DISPARITY,value)
            return valid
        else:
            print("Failed to initalise the pyI3DRSGM class. Make sure to initalise the class 'i3rsgm = pyI3DRSGM(...'")
            print("Check valid initalisation with 'isInit' function. E.g. 'i3rsgm.isInit()'")
            return False

    def enableInterpolation(self,enable):
        # Enable interpolation in I3DRSGM algorithm
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
        # Close connection to I3DRSGM app API
        # Required to clean up memory
        self.i3drsgmAppAPI.close()

if __name__ == '__main__':
    # Get folder containing current script
    script_folder = os.path.dirname(os.path.realpath(__file__))
    resource_folder = os.path.join(script_folder,"../resources")

    # Load stereo calibration from yamls
    left_cal_file = os.path.join(resource_folder,"sim_left.yaml")
    right_cal_file = os.path.join(resource_folder,"sim_right.yaml")
    stcal = StereoCalibration()
    stcal.get_cal_from_yaml(left_cal_file,right_cal_file)

    # Initalise I3DRSGM
    print("Intitalising I3DRSGM...")
    i3drsgm = pyI3DRSGM(license_file="2020_02_21_I3DRWL001_WINDOWS.lic")
    if (i3drsgm.isInit()):
        # Load images from file
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
                # Downsample disparity image for faster processing
                # Downsample rate is passed through to 'display_disp' and 'depth_from_disp' to compensative for the change in image size
                downsample_rate = 0.5
                disp_resize = cv2.resize(disp,None,fx=downsample_rate,fy=downsample_rate,interpolation=cv2.INTER_NEAREST)

                # Get normalised colormap for visualising disparity
                disp_colormap = colormap_from_disparity(disp_resize,Q,downsample_rate)

                # Resize colormap image for displaying in window
                disp_colormap_resized = image_resize(disp_colormap, height=640)
                # Display disparity colormap in OpenCV window
                cv2.imshow("display", disp_colormap_resized)

                # Calculate depth from disparity
                depth = depth_from_disp(disp_resize,Q,downsample_rate)

                print("Press any key on image window to close")
                cv2.waitKey(0)
                break # We only want to run this once for our purposes but can remove this to run continously

        # Important to close I3DRSGM to clean up memory
        # If program crashes before this has been called then you may need to restart the terminal to clean up memory
        i3drsgm.close()