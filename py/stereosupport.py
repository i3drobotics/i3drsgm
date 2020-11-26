import cv2
import numpy as np

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

def colormap_from_disparity(disp,Q,downsample_rate=1.0):
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

    return disp_colormap