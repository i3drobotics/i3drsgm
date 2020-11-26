import os
import cv2
import numpy as np
import scipy
import pcl

from stereosupport import colormap_from_disparity,depth_from_disp,image_resize
from Stereo3D import StereoCalibration
from pyi3drsgm import pyI3DRSGM

def cylinder(r, h, c=2, a =0, nt=100, nv =50):
    # Generate points of a cylinder
    # radius r, height h, base point a
    theta = np.linspace(0, c, nt)
    v = np.linspace(a, a+h, nv )
    theta, v = np.meshgrid(theta, v)
    x = r*np.cos(theta)
    y = r*np.sin(theta)
    z = v
    return x, y, z

def rotatePointsAxis(points,rotation,axis):
    # rotate points by axis
    rotation_degrees = rotation
    rotation_axis = np.array(axis)
    rotation_radians = np.radians(rotation_degrees)
    rotation_vector = rotation_radians * rotation_axis
    rotation = scipy.spatial.transform.Rotation.from_rotvec(rotation_vector)
    return rotation.apply(points)

def rotatePointsXYZ(points,rotation):
    # rotate points by xyz angle
    points_rot = rotatePointsAxis(points,rotation[0],[1,0,0])
    points_rot = rotatePointsAxis(points_rot,rotation[1],[0,1,0])
    points_rot = rotatePointsAxis(points_rot,rotation[2],[0,0,1])
    return points_rot

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

                cv2.waitKey(1)
                break # We only want to run this once for our purposes but can remove this to run continously

        # Important to close I3DRSGM to clean up memory
        # If program crashes before this has been called then you may need to restart the terminal to clean up memory
        i3drsgm.close()

        # Set min max values for cylinder radius look for in segmentation
        # When simulating depth will use this to define the cylinder size
        cylinder_radius_min = 0.05
        cylinder_radius_max = 0.08

        # Paramters used to generate cylinder
        sim_cylinder_height = 0.3
        sim_cylinder_radius = (cylinder_radius_min + cylinder_radius_max) / 2 # radius half way between cylinder search limits
        sim_cylinder_noise = 0.005
        sim_cylinder_rotation = [90,-180,60]
        sim_cylinder_translation = (0.1,0.3,1.5)
        sim_cylinder_complete_radius = 1*np.pi # how complete the cylinder radius is (2pi is complete radius, 1pi is half radius)

        # Simulate fake cylinder point cloud to test segmentation
        simulate_depth = True
        if (simulate_depth):
            print("Generating cylinder for testing...")
            # Generate cylindrical data with a radius of r on the bottom and a height of h.
            h = sim_cylinder_height
            r = sim_cylinder_radius
            nt = int(1000 * downsample_rate)
            nv = int(500 * downsample_rate)
            x,y,z = cylinder(r, h, sim_cylinder_complete_radius, 0, nt, nv)
            depth_sim = cv2.merge( (x, y, z) )

            # add some random noise to the cylinder to make searching more difficult
            noise = np.random.normal(loc=0, scale=sim_cylinder_noise, size=depth_sim.shape)
            depth_sim = depth_sim + noise

            # format points for use in pcl
            points_sim = depth_sim.astype(np.float32).reshape(-1, 3)
            points_real = depth.astype(np.float32).reshape(-1, 3)

            # rotate points
            points_sim = rotatePointsXYZ(points_sim,sim_cylinder_rotation)
            # translate points
            points_sim = points_sim + sim_cylinder_translation

            # make sure points are float32 for use in PointCloud
            points_sim = points_sim.astype(np.float32)

            # combine simulated cylinder with depth data from I3DRSGM result
            points = np.concatenate( (points_sim, points_real), axis=0)
        else:
            # Use real depth data from I3DRSGM result
            xyz = depth
            points = xyz.astype(np.float32).reshape(-1, 3)

        # Convert to PCL point cloud
        print("Preparing data for PCL...")
        cloud = pcl.PointCloud()
        cloud.from_array(points)

        # Filter out 0 < z > 10
        fil = cloud.make_passthrough_filter()
        fil.set_filter_field_name("z")
        fil.set_filter_limits(0.0001, 10)
        cloud_filtered = fil.filter()

        # Save current point cloud to file
        print("Saving test points to file...")
        pcl.save(cloud_filtered, os.path.join(script_folder,"tmp",'full_cloud.pcd'))
        
        # Run RANSAC Cylinder segmenter on normals
        print("Running PCL cyclinder segmentation...")
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
            print("Saving segmented cylinder points to file...")
            # Segment cloud based on segmentation indices
            cloud_cylinder = cloud_filtered.extract(indices, negative=False)
            # Save cloud segment to file
            pcl.save(cloud_cylinder, os.path.join(script_folder,"tmp",'cylinder_seg.pcd'))
        else:
            print("Segmenter failed")
        