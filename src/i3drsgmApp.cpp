#include <i3drsgm.h>
#include <opencv2/core/utils/filesystem.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <string>
#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef _WIN32
    #include <direct.h>
    #define GetCurrentDir _getcwd
#elif _linux__
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif

std::string getexepath()
{
    char cCurrentPath[FILENAME_MAX];
    GetCurrentDir(cCurrentPath, sizeof(cCurrentPath));
    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
    return std::string(cCurrentPath);
}

bool calcQ(
        cv::Mat l_proj_mat, cv::Mat r_proj_mat, cv::Mat l_camera_matrix,
        cv::Mat& Q){
    //calculate q
    Q = cv::Mat::zeros(4, 4, CV_64F);
    double cx = l_proj_mat.at<double>(0,2);
    double cxr = r_proj_mat.at<double>(0,2);
    double cy = l_proj_mat.at<double>(1,2);
    double fx = l_camera_matrix.at<double>(0,0);
    double fy = l_camera_matrix.at<double>(1,1);

    double p14 = r_proj_mat.at<double>(0,3);
    double baseline = -p14 / fx;

    double q33 = -(cx - cxr) / baseline;

    Q.at<double>(0,0) = 1.0;
    Q.at<double>(0,3) = -cx;
    Q.at<double>(1,1) = 1.0;
    Q.at<double>(1,3) = -cy;
    Q.at<double>(2,3) = fx;
    Q.at<double>(3,2) = 1.0 / baseline;
    Q.at<double>(3,3) = q33;

    Q.convertTo(Q, CV_32F);
    return true;
}

void generateRectificationMap(
        cv::Mat camera_matrix, cv::Mat dist_coeffs, cv::Mat rect_mat, cv::Mat proj_mat, cv::Size image_size, 
        cv::Mat& rectmapx, cv::Mat& rectmapy){
    cv::initUndistortRectifyMap(camera_matrix, dist_coeffs, rect_mat, proj_mat, image_size, CV_32FC1, rectmapx, rectmapy);
}

bool loadCalibrationFromYaml(
        std::string filepath,
        cv::Mat& camera_matrix, cv::Mat& dist_coeffs, cv::Mat& rect_mat, cv::Mat& proj_mat, 
        cv::Size& cal_image_size, cv::Mat& rectmapx, cv::Mat& rectmapy) {
    
    if (!cv::utils::fs::exists(filepath)){
        std::cerr << "calibration file:' " << filepath << " ' does not exist" << std::endl;
        return false;
    }

    int flags = cv::FileStorage::READ;
    cv::FileStorage fs_l(filepath, flags);

    int cal_image_width = 0;
    int cal_image_height = 0;

    if (fs_l.isOpened()) {
        fs_l["image_width"] >> cal_image_width;
        fs_l["image_height"] >> cal_image_height;
        fs_l["camera_matrix"] >> camera_matrix;
        fs_l["distortion_coefficients"] >> dist_coeffs;
        fs_l["rectification_matrix"] >> rect_mat;
        fs_l["projection_matrix"] >> proj_mat;
    } else {
        return false;
    }

    cal_image_size = cv::Size(cal_image_width,cal_image_height);

    fs_l.release();

    std::cout << "Pre-computing rectification maps..." << std::endl;
    generateRectificationMap(camera_matrix, dist_coeffs, rect_mat, proj_mat, cal_image_size, rectmapx, rectmapy);

    std::cout << "Camera matrix:" << std::endl;
    std::cout << camera_matrix << std::endl;

    std::cout << "Distortion Coefficients:" << std::endl;
    std::cout << dist_coeffs << std::endl;

    std::cout << "Rectification matrix:" << std::endl;
    std::cout << rect_mat << std::endl;

    std::cout << "Projection matrix:" << std::endl;
    std::cout << proj_mat << std::endl;

    return true;
}

bool loadCalibrationFromXml(
        std::string filepath,
        cv::Mat& camera_matrix, cv::Mat& dist_coeffs){

    if (!cv::utils::fs::exists(filepath)){
        std::cerr << "calibration file:' " << filepath << " ' does not exist" << std::endl;
        return false;
    }

    int flags = cv::FileStorage::READ;
    cv::FileStorage fs_l(filepath, flags);       

    if (fs_l.isOpened()) {
        fs_l["cameraMatrix"] >> camera_matrix;
        fs_l["distCoeffs"] >> dist_coeffs;
    } else {
        return false;
    }

    fs_l.release();

    std::cout << "Camera matrix:" << std::endl;
    std::cout << camera_matrix << std::endl;

    std::cout << "Distortion Coefficients:" << std::endl;
    std::cout << dist_coeffs << std::endl;

    return true;
}

bool loadCalibrationFromXmls(
        std::string left_cal, std::string right_cal,std::string stereo_cal, cv::Size image_size, 
        cv::Mat& l_camera_matrix, cv::Mat& l_dist_coeffs, cv::Mat& l_rect_mat, cv::Mat& l_proj_mat, cv::Mat& l_rectmapx, cv::Mat& l_rectmapy,
        cv::Mat& r_camera_matrix, cv::Mat& r_dist_coeffs, cv::Mat& r_rect_mat, cv::Mat& r_proj_mat, cv::Mat& r_rectmapx, cv::Mat& r_rectmapy,
        cv::Mat& Q) {
    
    bool valid = true;
    std::cout << "Loading left calibration..." << std::endl;
    valid &= loadCalibrationFromXml(left_cal,l_camera_matrix,l_dist_coeffs);
    std::cout << "Loading right calibration..." << std::endl;
    valid &= loadCalibrationFromXml(right_cal,r_camera_matrix,r_dist_coeffs);

    std::cout << "Left camera matrix:" << std::endl;
    std::cout << l_camera_matrix << std::endl;

    std::cout << "Left distortion Coefficients:" << std::endl;
    std::cout << l_dist_coeffs << std::endl;

    std::cout << "Loading stereo calibration..." << std::endl;
    if (!cv::utils::fs::exists(stereo_cal)){
        std::cerr << "calibration file:' " << stereo_cal << " ' does not exist" << std::endl;
        return false;
    }

    int flags = cv::FileStorage::READ;
    cv::FileStorage fs_s(stereo_cal, flags);

    cv::Mat T, R;

    if (fs_s.isOpened()) {
        fs_s["Q"] >> Q;
        fs_s["T"] >> T;
        fs_s["R"] >> R;
        Q.convertTo(Q, CV_32F);
    } else {
        return false;
    }

    fs_s.release();

    std::cout << "Q:" << std::endl;
    std::cout << Q << std::endl;

    std::cout << "R:" << std::endl;
    std::cout << R << std::endl;

    std::cout << "T:" << std::endl;
    std::cout << T << std::endl;

    cv::Mat Q_cal;

    std::cout << "Pre-computing stereo values..." << std::endl;
    cv::stereoRectify(
        l_camera_matrix, l_dist_coeffs, r_camera_matrix, r_dist_coeffs, image_size, 
        R, T, l_rect_mat, r_rect_mat, l_proj_mat, r_proj_mat, Q_cal, 0);

    std::cout << "Pre-computing rectification maps..." << std::endl;
    generateRectificationMap(l_camera_matrix, l_dist_coeffs, l_rect_mat, l_proj_mat, image_size, l_rectmapx, l_rectmapy);
    generateRectificationMap(r_camera_matrix, r_dist_coeffs, r_rect_mat, r_proj_mat, image_size, r_rectmapx, r_rectmapy);

    return true;
}

bool loadCalibrationFromYamls(
        std::string left_cal_filepath, std::string right_cal_filepath,
        cv::Mat& l_camera_matrix, cv::Mat& l_dist_coeffs, cv::Mat& l_rect_mat, cv::Mat& l_proj_mat, cv::Mat& l_rectmapx, cv::Mat& l_rectmapy,
        cv::Mat& r_camera_matrix, cv::Mat& r_dist_coeffs, cv::Mat& r_rect_mat, cv::Mat& r_proj_mat, cv::Mat& r_rectmapx, cv::Mat& r_rectmapy,
        cv::Size& cal_image_size, cv::Mat& Q){
    bool valid = true;
    std::cout << "Loading left calibration..." << std::endl;
    valid &= loadCalibrationFromYaml(left_cal_filepath,l_camera_matrix,l_dist_coeffs,l_rect_mat,l_proj_mat,cal_image_size,l_rectmapx, l_rectmapy);
    std::cout << "Loading right calibration..." << std::endl;
    valid &= loadCalibrationFromYaml(right_cal_filepath,r_camera_matrix,r_dist_coeffs,r_rect_mat,r_proj_mat,cal_image_size,r_rectmapx, r_rectmapy);
    std::cout << "Computing Q matrix..." << std::endl;
    valid &= calcQ(l_proj_mat,r_proj_mat,l_camera_matrix,Q);

    return valid;
}

int main(int argc, char *argv[]){
    bool valid = I3DRSGM::isLicenseValid();
    valid = true; //TODO remove this
    if (!valid){
        std::string hostname, hostid;
        I3DRSGM::getHostInfo(hostname,hostid);
        std::cerr << "Invalid license. Please place license file in the following directory: " << std::endl;
        std::cerr << "'"<< getexepath() << "'" << std::endl;
        std::cerr << std::endl;
        std::cerr << "If you do not have a license, contact info@i3drobotics.com to purchase a license and provide the following details: " << std::endl;
        std::cerr << "Hostname: " << hostname << " HostID: " << hostid << std::endl;
    	return -1;
    } else {
        // default values
        std::string left_filepath = "../../resources/left.png";
        std::string right_filepath = "../../resources/right.png";
        std::string calibration_type = "xml"; // yaml or xml
        std::string left_cal_filepath = "../../resources/left.xml"; //left.yaml or left.xml
        std::string right_cal_filepath = "../../resources/right.xml"; //right.yaml or right.xml
        std::string stereo_cal_filepath = "../../resources/stereo.xml"; //xml only
        std::string tmp_param_filepath = getexepath()+"/i3drsgm_tmp.param";
        std::string param_filepath = getexepath()+"/i3drsgm.param";
        std::string left_rect_filepath = "../../resources/left_rect.png";
        std::string right_rect_filepath = "../../resources/right_rect.png";
        std::string disp_filepath = "../../resources/disp.tiff";
        std::string disp_norm_filepath = "../../resources/disp_norm.png";
        std::string depth_filepath = "../../resources/depth.tiff";
        std::string point_cloud_filepath = "../../resources/points.ply";
        bool rectify = true;
        bool output_rectify = true;
        bool output_disp = true;
        bool output_norm_disp = true;
        bool output_depth = true;
        bool output_points = true;

        // read input.yaml. should include:
            // INPUTS
            // left, right image filepath (String default:'left.png','right.png')
            // calibration type (String 'xml'/'yaml' default:'yaml')
            // left, right calibration filepath (String default:'left.yaml','right.yaml')
            // i3drsgm param filepath (String default:'i3drsgm.param')
            // OUTPUTS
            // left, right rectified image filepath (String default:'left_rect.png','right_rect.png')
            // disparity image filename (String default:"disp.tiff")
            // normalised disparity image filename (String default:"disp_norm.png")
            // depth image filename (String default:"depth.tiff")
            // rectify images (Boolean default:true)
            // save rectify images (Boolean default:true)
            // save disparity image (Boolean default:true)
            // save normalised disparity image (Boolean default:true)
            // save depth image (Boolean default:true)
            // save point cloud (Boolean default:true)

        if (calibration_type != "yaml" && calibration_type !="xml"){
            std::cerr << "Invalid calibration type. MUST be 'yaml' or 'xml'" << std::endl;
            return -1;
        }

        // initalise matcher with param files
        I3DRSGM * i3drsgm = new I3DRSGM(tmp_param_filepath,param_filepath);

        cv::Mat left, right;
        // check image files exist
        if (cv::utils::fs::exists(left_filepath) && cv::utils::fs::exists(right_filepath)){
            // read left and right image from file to OpenCV Mat
            left = cv::imread(left_filepath,cv::IMREAD_UNCHANGED);
            right = cv::imread(right_filepath,cv::IMREAD_UNCHANGED);
        } else {
            std::cerr << "Image file does not exist" << std::endl;
            return -1;
        }
        
        // check calibration files exist (xml or yaml)
        bool cal_valid = true;
        cv::Mat l_camera_matrix, r_camera_matrix;
        cv::Mat l_dist_coef, r_dist_coef;
        cv::Mat l_rect_matrix, r_rect_matrix;
        cv::Mat l_proj_matrix, r_proj_matrix;
        cv::Mat l_rectmapx, r_rectmapx;
        cv::Mat l_rectmapy, r_rectmapy;
        cv::Mat Q;
        cv::Size cal_image_size;
        std::cout << "Loading calibration files..." << std::endl;
        if (calibration_type == "yaml"){
            cal_valid &= loadCalibrationFromYamls(
                left_cal_filepath,right_cal_filepath,
                l_camera_matrix,l_dist_coef,l_rect_matrix,l_proj_matrix,l_rectmapx,l_rectmapy,
                r_camera_matrix,r_dist_coef,r_rect_matrix,r_proj_matrix,r_rectmapx,r_rectmapy,
                cal_image_size,Q);
        } else if (calibration_type == "xml"){
            cal_valid &= loadCalibrationFromXmls(
                left_cal_filepath,right_cal_filepath,stereo_cal_filepath, left.size(),
                l_camera_matrix,l_dist_coef,l_rect_matrix,l_proj_matrix,l_rectmapx,l_rectmapy,
                r_camera_matrix,r_dist_coef,r_rect_matrix,r_proj_matrix,r_rectmapx,r_rectmapy, Q);
        }

        if (!cal_valid){
            std::cerr << "Invalid calibration file/s" << std::endl;
            return -1;
        }
        
        if (rectify){
            // rectify input images
            std::cout << "Rectifying images..." << std::endl;
            cv::Mat left_rect, right_rect;
            cv::remap(left, left_rect, l_rectmapx, l_rectmapy, cv::INTER_CUBIC);
            cv::remap(right, right_rect, r_rectmapx, r_rectmapy, cv::INTER_CUBIC);
            if (output_rectify){
                // output rectified images to file
                cv::imwrite(left_rect_filepath,left);
                cv::imwrite(right_rect_filepath,right);
            }
        }
        
        // Run stereo match on left and right images
        std::cout << "Generating disparity from stereo pair..." << std::endl;
        cv::Mat disp = i3drsgm->forwardMatch(left,right);

        if (output_disp){
            // save disparity image to tiff file
        }
        
        if (output_norm_disp){
            // normalise disparity
            // apply colormap to normalised disparity
            // save normalised with colormap to png file
        }

        if (output_depth || output_points){
            // generate depth from disparity
            std::cout << "Generating depth from disparity..." << std::endl;
            cv::Mat depth;
            cv::reprojectImageTo3D(disp,depth,Q);
            // save depth image to tiff file
            if (output_points){
                // generate point cloud from depth image
                // save point cloud to file
            }
        }

        std::cout << "Complete." << std::endl;
    	return 0;
    }
}