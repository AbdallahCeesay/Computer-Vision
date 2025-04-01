//James Rogers Jan 2024 (c) Plymouth University
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <chrono>

#include "../owl.h"

using namespace cv;
using namespace std;
using namespace std::chrono;

int main(int argc, char** argv)
{
    //connect with the owl and load calibration values
    robotOwl owl(1560, 1625, 1500, 1470, 1520);

    //Calibration file paths
    string intrinsic_filename = "../intrinsics.xml";
    string extrinsic_filename = "../extrinsics.xml";

    //================================================Load Calibration Files===============================================
    Rect roi1, roi2;
    Mat Q;
    Size img_size = {640,480};

    FileStorage fs(intrinsic_filename, FileStorage::READ);
    if(!fs.isOpened()){
        printf("Failed to open file %s\n", intrinsic_filename.c_str());
        return -1;
    }

    Mat M1, D1, M2, D2;
    fs["M1"] >> M1;
    fs["D1"] >> D1;
    fs["M2"] >> M2;
    fs["D2"] >> D2;

    fs.open(extrinsic_filename, FileStorage::READ);
    if(!fs.isOpened())
    {
        printf("Failed to open file %s\n", extrinsic_filename.c_str());
        return -1;
    }
    Mat R, T, R1, P1, R2, P2;
    fs["R"] >> R;
    fs["T"] >> T;

    stereoRectify( M1, D1, M2, D2, img_size, R, T, R1, R2, P1, P2, Q, CALIB_ZERO_DISPARITY, -1, img_size, &roi1, &roi2 );

    Mat map11, map12, map21, map22;
    initUndistortRectifyMap(M1, D1, R1, P1, img_size, CV_16SC2, map11, map12);
    initUndistortRectifyMap(M2, D2, R2, P2, img_size, CV_16SC2, map21, map22);

    //==================================================Create Block Matcher==============================================
    int SADWindowSize=9;            //must be an odd number >=3
    int numberOfDisparities=144;    //must be divisible by 16
    Ptr<StereoSGBM> sgbm = StereoSGBM::create(0,16,3);
    sgbm->setBlockSize(SADWindowSize);
    sgbm->setPreFilterCap(40);
    sgbm->setP1(8*3*SADWindowSize*SADWindowSize);
    sgbm->setP2(32*3*SADWindowSize*SADWindowSize);
    sgbm->setMinDisparity(0);
    sgbm->setNumDisparities(numberOfDisparities);
    sgbm->setUniquenessRatio(30);
    sgbm->setSpeckleWindowSize(200);
    sgbm->setSpeckleRange(30);
    sgbm->setDisp12MaxDiff(1);
    sgbm->setMode(StereoSGBM::MODE_SGBM);

    auto start_time = high_resolution_clock::now();
    int frame_count = 0;
    double total_disparity = 0;

    //Main program loop
    while (1){
        //read the owls camera frames
        Mat left, right;
        owl.getCameraFrames(left, right);

        //Distort images to correct for lens/positional distortion
        remap(left, left, map11, map12, INTER_LINEAR);
        remap(right, right, map21, map22, INTER_LINEAR);

        //Match left and right images to create disparity image
        Mat disp, disp8;
        sgbm->compute(left, right, disp);

        //Convert disparity map to an 8-bit greyscale image so it can be displayed (do not use for measurements)
        disp.convertTo(disp8, CV_8U, 255/(numberOfDisparities*16.));

        // Find the center point of the image
        Point center(disp.cols / 2, disp.rows / 2);

        // Get disparity value at center
        short disparity_value = disp.at<short>(center);
        cout << "Disparity at center (" << center.x << ", " << center.y << "): " << disparity_value << endl;

        // Accumulate disparity values
        total_disparity += disparity_value;
        frame_count++;

        // Check elapsed time
        auto current_time = high_resolution_clock::now();
        double elapsed_seconds = duration_cast<seconds>(current_time - start_time).count();
        if (elapsed_seconds >= 5.0) {
            double avg_disparity = total_disparity / frame_count;
            cout << "Average disparity over last " << elapsed_seconds << " seconds: " << avg_disparity << endl;
            start_time = high_resolution_clock::now();
            frame_count = 0;
            total_disparity = 0;
        }

        // Draw a marker at the center
        drawMarker(disp8, center, Scalar(255), MARKER_CROSS, 10, 2);
        drawMarker(left, center, Scalar(0, 0, 255), MARKER_CROSS, 10, 2);

        //display images
        imshow("left", left);
        imshow("right", right);
        imshow("disparity", disp8);
        waitKey(10);
    }

    return 0;
}
