#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/core/utils/filesystem.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>
#include <stdio.h>  /* defines FILENAME_MAX */

class StereoSupport
{
public:
    static bool calcQ(
        cv::Mat l_proj_mat, cv::Mat r_proj_mat, cv::Mat l_camera_matrix,
        cv::Mat& Q){
        //calculate q
        Q = cv::Mat::zeros(4, 4, CV_64F);
        double cx = l_proj_mat.at<double>(0,2);
        double cxr = r_proj_mat.at<double>(0,2);
        double cy = l_proj_mat.at<double>(1,2);
        double fx = l_camera_matrix.at<double>(0,0);
        //double fy = l_camera_matrix.at<double>(1,1);

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

    static void generateRectificationMap(
            cv::Mat camera_matrix, cv::Mat dist_coeffs, cv::Mat rect_mat, cv::Mat proj_mat, cv::Size image_size, 
            cv::Mat& rectmapx, cv::Mat& rectmapy){
        cv::initUndistortRectifyMap(camera_matrix, dist_coeffs, rect_mat, proj_mat, image_size, CV_32FC1, rectmapx, rectmapy);
    }

    static bool loadCalibrationFromYaml(
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

        //std::cout << "Pre-computing rectification maps..." << std::endl;
        generateRectificationMap(camera_matrix, dist_coeffs, rect_mat, proj_mat, cal_image_size, rectmapx, rectmapy);

        //std::cout << "Camera matrix:" << std::endl;
        //std::cout << camera_matrix << std::endl;

        //std::cout << "Distortion Coefficients:" << std::endl;
        //std::cout << dist_coeffs << std::endl;

        //std::cout << "Rectification matrix:" << std::endl;
        //std::cout << rect_mat << std::endl;

        //std::cout << "Projection matrix:" << std::endl;
        //std::cout << proj_mat << std::endl;

        return true;
    }

    static bool loadCalibrationFromXml(
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

        //std::cout << "Camera matrix:" << std::endl;
        //std::cout << camera_matrix << std::endl;

        //std::cout << "Distortion Coefficients:" << std::endl;
        //std::cout << dist_coeffs << std::endl;

        return true;
    }

    static bool loadCalibrationFromXmls(
            std::string left_cal, std::string right_cal,std::string stereo_cal, cv::Size image_size, 
            cv::Mat& l_camera_matrix, cv::Mat& l_dist_coeffs, cv::Mat& l_rect_mat, cv::Mat& l_proj_mat, cv::Mat& l_rectmapx, cv::Mat& l_rectmapy,
            cv::Mat& r_camera_matrix, cv::Mat& r_dist_coeffs, cv::Mat& r_rect_mat, cv::Mat& r_proj_mat, cv::Mat& r_rectmapx, cv::Mat& r_rectmapy,
            cv::Mat& Q) {
        
        bool valid = true;
        //std::cout << "Loading left calibration..." << std::endl;
        valid &= loadCalibrationFromXml(left_cal,l_camera_matrix,l_dist_coeffs);
        //std::cout << "Loading right calibration..." << std::endl;
        valid &= loadCalibrationFromXml(right_cal,r_camera_matrix,r_dist_coeffs);

        //std::cout << "Left camera matrix:" << std::endl;
        //std::cout << l_camera_matrix << std::endl;

        //std::cout << "Left distortion Coefficients:" << std::endl;
        //std::cout << l_dist_coeffs << std::endl;

        //std::cout << "Loading stereo calibration..." << std::endl;
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

        //std::cout << "Q:" << std::endl;
        //std::cout << Q << std::endl;

        //std::cout << "R:" << std::endl;
        //std::cout << R << std::endl;

        //std::cout << "T:" << std::endl;
        //std::cout << T << std::endl;

        cv::Mat Q_cal;

        //std::cout << "Pre-computing stereo values..." << std::endl;
        cv::stereoRectify(
            l_camera_matrix, l_dist_coeffs, r_camera_matrix, r_dist_coeffs, image_size, 
            R, T, l_rect_mat, r_rect_mat, l_proj_mat, r_proj_mat, Q_cal, 0);

        //std::cout << "Pre-computing rectification maps..." << std::endl;
        generateRectificationMap(l_camera_matrix, l_dist_coeffs, l_rect_mat, l_proj_mat, image_size, l_rectmapx, l_rectmapy);
        generateRectificationMap(r_camera_matrix, r_dist_coeffs, r_rect_mat, r_proj_mat, image_size, r_rectmapx, r_rectmapy);

        return true;
    }

    static bool loadCalibrationFromYamls(
            std::string left_cal_filepath, std::string right_cal_filepath,
            cv::Mat& l_camera_matrix, cv::Mat& l_dist_coeffs, cv::Mat& l_rect_mat, cv::Mat& l_proj_mat, cv::Mat& l_rectmapx, cv::Mat& l_rectmapy,
            cv::Mat& r_camera_matrix, cv::Mat& r_dist_coeffs, cv::Mat& r_rect_mat, cv::Mat& r_proj_mat, cv::Mat& r_rectmapx, cv::Mat& r_rectmapy,
            cv::Size& cal_image_size, cv::Mat& Q){
        bool valid = true;
        //std::cout << "Loading left calibration..." << std::endl;
        valid &= loadCalibrationFromYaml(left_cal_filepath,l_camera_matrix,l_dist_coeffs,l_rect_mat,l_proj_mat,cal_image_size,l_rectmapx, l_rectmapy);
        //std::cout << "Loading right calibration..." << std::endl;
        valid &= loadCalibrationFromYaml(right_cal_filepath,r_camera_matrix,r_dist_coeffs,r_rect_mat,r_proj_mat,cal_image_size,r_rectmapx, r_rectmapy);
        //std::cout << "Computing Q matrix..." << std::endl;
        valid &= calcQ(l_proj_mat,r_proj_mat,l_camera_matrix,Q);

        return valid;
    }
};
