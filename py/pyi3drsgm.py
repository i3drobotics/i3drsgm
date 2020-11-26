import cv2
import numpy as np
import subprocess
import os
from Stereo3D import Stereo3D, StereoCalibration
from Stereo3D.StereoCapture import *
import pyransac3d as pyrsc
import open3d as o3d
import copy
import pcl
import math
import scipy

class I3DRSGMAppAPI:
    def __init__(self, output_folder=None, I3RSGMApp_folder=None):
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

    def __init__(self, tmp_folder=None, I3RSGMApp_folder=None):
        # Initalse I3DRSGM
        # Initalise connection to I3DRSGM app API
        self.i3drsgmAppAPI = I3DRSGMAppAPI(tmp_folder, I3RSGMApp_folder)

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

def scale_disparity(disparity):
    # Normalise disparity by min and max value in image
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

def reprojectImageTo3D(disp,Q,downsample_rate=1.0):
    # Get important values from Q matrix
    wz = Q[2, 3]
    q03 = Q[0, 3]
    q13 = Q[1, 3]
    q32 = Q[3, 2]
    q33 = Q[3, 3]

    # Calculate downsample factor to correct for downsampling applied to disparity image
    # This make sure that the disparity value is not effected by the donwnsampling
    downsample_factor = 1/downsample_rate

    # Calculate W from key values in Q matrix
    w = ( disp * q32) + q33
    # Calculate Z channel of depth image
    z = wz / w
    # Fill x,y channel the same size as Z
    y = np.full_like(z, 1)
    x = np.full_like(z, 1)

    # Calculate x and y elements
    # Downsample factor is applied to compensate for using a downsampled disparity image
    # this is needed as the index of the pixel (i and j) is used in the calculation
    num_rows, num_cols = disp.shape
    for i in range(0, num_rows):
        for j in range(0, num_cols):
            x[i,j] = ((j * downsample_factor) + q03) / w[i,j]
            y[i,j] = ((i * downsample_factor) + q13) / w[i,j]

    # Combine x,y,z into depth image
    depth = cv2.merge((x,y,z))

    return depth

def depth_from_disp(disp,Q,downsample_rate=1.0):
    
    # Get important values from Q matrix
    q32 = Q[3, 2]
    q33 = Q[3, 3]

    # I3DRSGM returns negative disparity so invert
    disparity16 = -disp.astype(np.float32)

    # Calculate W from key values in Q matrix
    w = (disparity16 * q32) + q33

    # Find min and max W
    minW = np.min(w)
    maxW = np.max(w)

    # Find elements in W less than 0 (invalid as would be behind camera)
    w_zero_mask = w <= 0
    # Filter disparity image to only allow valid disparities
    disparity16[w_zero_mask != 0] = 0.0

    # Find elements in W eq to 99999 (invalid disparity signifier)
    d_inf_mask = disparity16 == 99999
    # Filter disparity image to only allow valid disparities
    disparity16[d_inf_mask != 0] = 0.0

    # Calculate min max disparity (ignoring zeros)
    masked_a = np.ma.masked_equal(disparity16, 0.0, copy=False)
    minDisp = masked_a.min()
    maxDisp = masked_a.max()

    # Replace invalid disparities with minium disparity
    disparity16[w_zero_mask != 0] = minDisp
    # Replace invalid disparities with maximum disparity
    disparity16[d_inf_mask != 0] = maxDisp

    # Generate depth from disparity
    print("Generating depth from disparity...")
    depth = reprojectImageTo3D(disparity16, Q, downsample_rate)
    
    # Filter depth image to only allow valid disparities
    depth[w_zero_mask != 0] = [ 0, 0, 0]

    # Split depth image into x,y,z channels
    x,y,z = cv2.split(depth)
    # Filter z to remove invalid values
    z[w_zero_mask != 0] = 0.0
    z[d_inf_mask != 0] = 0.0
    # Re-combine depth image
    depth = cv2.merge((x,y,z))

    # Calculate min max depth
    masked_depth = np.ma.masked_equal(z, 0.0, copy=False)
    minDepth = masked_depth.min()
    maxDepth = masked_depth.max()
    print("Depth range: "+str(minDepth)+"m, "+str(maxDepth)+"m")

    return depth

def display_disp(disp,Q,downsample_rate=1.0):
    # Display normalised disparity with colormap in OpenCV windows

    # Get important values from Q matrix
    q32 = Q[3, 2]
    q33 = Q[3, 3]

    # I3DRSGM returns negative disparity so invert
    disparity16 = -disp.astype(np.float32)
    
    # Calculate W from key values in Q matrix
    w = (disparity16* q32) + q33

    # Find min and max W
    minW = np.min(w)
    maxW = np.max(w)

    # Find elements in W less than 0 (invalid as would be behind camera)
    w_zero_mask = w <= 0
    # Filter disparity image to only allow valid disparities
    disparity16[w_zero_mask != 0] = 0.0

    # Find elements in W eq to 99999 (invalid disparity signifier)
    d_inf_mask = disparity16 == 99999
    # Filter disparity image to only allow valid disparities
    disparity16[d_inf_mask != 0] = 0.0

    # Calculate min max disparity (ignoring zeros)
    masked_a = np.ma.masked_equal(disparity16, 0.0, copy=False)
    minDisp = masked_a.min()
    maxDisp = masked_a.max()

    # Replace invalid disparities with minium disparity
    disparity16[w_zero_mask != 0] = minDisp
    # Replace invalid disparities with maximum disparity
    disparity16[d_inf_mask != 0] = maxDisp
    # Normalise disparity
    disp_scaled = scale_disparity(disparity16)

    print("Applying colormap to disparity...")

    # Apply color map to disparity
    disp_colormap = cv2.applyColorMap(disp_scaled, cv2.COLORMAP_JET)
    # Filter out invalid disparities from colormap
    disp_colormap[w_zero_mask != 0] = [0, 0, 0]
    disp_colormap[d_inf_mask != 0] = [0, 0, 0]

    # Resize colormap image for displaying in window
    disp_colormap_resized = image_resize(disp_colormap, height=640)
    # Display disparity colormap in OpenCV window
    cv2.imshow("display", disp_colormap_resized)
    cv2.waitKey(1)

def cylinder(r, h, c=2, a =0, nt=100, nv =50):
    # Generate points of a cylinder
    # radius r, height h, base point a
    theta = np.linspace(0, c*np.pi, nt)
    v = np.linspace(a, a+h, nv )
    theta, v = np.meshgrid(theta, v)
    x = r*np.cos(theta)
    y = r*np.sin(theta)
    z = v
    return x, y, z

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
    i3drsgm = pyI3DRSGM()
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
                downsample_rate = 0.1
                disp_resize = cv2.resize(disp,None,fx=downsample_rate,fy=downsample_rate,interpolation=cv2.INTER_NEAREST)
                # Display normalised disparity in OpenCV window
                display_disp(disp_resize,Q,downsample_rate)
                # Calculate depth from disparity
                depth = depth_from_disp(disp_resize,Q,downsample_rate)
                break # We only want to run this once for our purposes but can remove this to run continously

        # Important to close I3DRSGM to clean up memory
        # If program crashes before this has been called then you may need to restart the terminal to clean up memory
        i3drsgm.close()

        # Set min max values for cylinder radius look for in segmentation
        # When simulating depth will use this to define the cylinder size
        cylinder_radius_min = 0.1
        cylinder_radius_max = 0.2

        # Simulate fake cylinder point cloud to test segmentation
        simulate_depth = True
        if (simulate_depth):
            # Generate cylindrical data with a radius of r on the bottom and a height of h.
            h = 0.5
            r = (cylinder_radius_min + cylinder_radius_max) / 2 # radius half way between cylinder search limits
            x,y,z = cylinder(r, h, 2)
            depth_sim = cv2.merge( (x, y, z) )

            # add some random noise to the cylinder to make searching more difficult
            noise = np.random.normal(loc=0, scale=0.01, size=depth_sim.shape)
            depth_sim = depth_sim + noise

            # format points for use in pcl
            points_sim = depth_sim.astype(np.float32).reshape(-1, 3)
            points_real = depth.astype(np.float32).reshape(-1, 3)

            # rotate points
            rotation_degrees = 30
            rotation_axis = np.array([0, 1, 0])
            rotation_radians = np.radians(rotation_degrees)
            rotation_vector = rotation_radians * rotation_axis
            rotation = scipy.spatial.transform.Rotation.from_rotvec(rotation_vector)
            points_sim = rotation.apply(points_sim)
            # translate points
            points_sim = points_sim + (0,0,0.5)

            # make sure points are float32 for use in PointCloud
            points_sim = points_sim.astype(np.float32)

            # combine simulated cylinder with depth data from I3DRSGM result
            points = np.concatenate( (points_sim, points_real), axis=0)
        else:
            # Use real depth data from I3DRSGM result
            xyz = depth
            points = xyz.astype(np.float32).reshape(-1, 3)

        # Convert to PCL point cloud
        print("PCL Segmentation")
        cloud = pcl.PointCloud()
        cloud.from_array(points)

        # Filter out 0 < z > 10
        fil = cloud.make_passthrough_filter()
        fil.set_filter_field_name("z")
        fil.set_filter_limits(0.0001, 10)
        cloud_filtered = fil.filter()

        # Save current point cloud to file
        pcl.save(cloud_filtered, os.path.join(script_folder,"tmp",'full_cloud.pcd'))
        
        # Run RANSAC Cylinder segmenter on normals
        seg = cloud_filtered.make_segmenter_normals(ksearch=50)
        seg.set_optimize_coefficients(True)
        seg.set_model_type(pcl.SACMODEL_CYLINDER)
        seg.set_normal_distance_weight(0.1)
        seg.set_method_type(pcl.SAC_RANSAC)
        seg.set_max_iterations(10000)
        seg.set_distance_threshold(0.05)
        seg.set_radius_limits(cylinder_radius_min, cylinder_radius_max)
        indices, model = seg.segment()
        
        if (len(indices) != 0): #check segment completed successfully
            print(model)
            # Segment cloud based on segmentation indices
            cloud_cylinder = cloud_filtered.extract(indices, negative=False)
            # Save cloud segment to file
            pcl.save(cloud_cylinder, os.path.join(script_folder,"tmp",'cylinder_seg.pcd'))
        else:
            print("Segmenter failed")
        