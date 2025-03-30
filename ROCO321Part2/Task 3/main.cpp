//James Rogers Nov 2023 (c) Plymouth University

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <iostream>
#include <string>
#include <cmath>

#include "../owl.h"

using namespace std;
using namespace cv;

#define targetSize 50 //adjust this to change the size of your target

int main()
{
    //connect with the owl and load calibration values
    robotOwl owl(1500, 1475, 1520, 1525, 1520);

    //===========================target selection loop==============================
    Mat target; // variable to store the template image
    Rect targetPos(320-targetSize/2, 240-targetSize/2, targetSize, targetSize);
    while (true){
        //read the owls camera frames
        Mat left, right; //source images are stored in left and right variables
        owl.getCameraFrames(left, right);

        //if SPACE is pressed, save the target image and break the loop
        if(waitKey(10)==' ')
        {
            left(targetPos).copyTo(target);
            cout<<"Target selected!"<<endl;
            break;
        }

        //display camera frame
        rectangle(left, targetPos, Scalar(255,255,255), 2);
        imshow("left",left);
    }

    //===========================target tracking loop================================
    cout<<"Starting tracking code..."<<endl;

    while(1)
    {
        //read the owls camera frames
        Mat left, right, matchOutputLeft, matchOutputRight;
        owl.getCameraFrames(left, right);

        /*========================== matchTemplate for the left eye =================================*/
        matchTemplate(left, target, matchOutputLeft, TM_SQDIFF_NORMED);
        double minValueLeft, maxValue;
        Point minLocationLeft, maxLocationLeft;
        minMaxLoc(matchOutputLeft, &minValueLeft, &maxValue, &minLocationLeft, &maxLocationLeft);


        //=========================== matchTemplate for the right eye ==============================

        matchTemplate(right, target, matchOutputRight, TM_SQDIFF_NORMED);
        double minValueRight, maxValueRight;
        Point minLocationRight, maxLocationRight;
        minMaxLoc(matchOutputRight, &minValueRight, &maxValueRight, &minLocationRight, &maxLocationRight);


        //================================ Threshold =======================================
        double visibilityThreshold = 0.25;
        int trackingThreshold = 60; // size of the square around the centre


        // ================================ Calculate Target Centres ==================================
        Point targetCentreLeft(minLocationLeft.x + target.cols / 2, minLocationLeft.y + target.rows / 2);
        Point targetCentreRight(minLocationRight.x + target.cols / 2, minLocationRight.y + target.rows / 2);


        //========================== check target visiblity in both camera frames ===============================
        bool targetVisibleLeft = minValueLeft < visibilityThreshold;
        bool targetVisibleRight = minValueRight < visibilityThreshold;

        if(targetVisibleLeft || targetVisibleRight) {

            //Draw markers on the frame
            Scalar markerColour(0, 0, 255);
            int markerSize = 15, markerThickness = 5;

            drawMarker(left, targetCentreLeft, markerColour, MARKER_CROSS, markerSize, markerThickness);            // marker on left camera frame
            drawMarker(right, targetCentreRight, markerColour, MARKER_CROSS, markerSize, markerThickness);          // marker on right camera frame


            // LEFT CAMERA TRACKING
            Point leftCamera_CentrePoint(left.size().width / 2, left.size().height / 2);
            int leftCameraCentre_x = leftCamera_CentrePoint.x;
            int leftCameraCentre_y = leftCamera_CentrePoint.y;

            int leftCamera_dx = (targetCentreLeft.x - leftCameraCentre_x);
            int leftCamera_dy = (targetCentreLeft.y - leftCameraCentre_y);

            // RIGHT CAMERA TRACKING
            Point rightCamera_CentrePoint(right.size().width / 2, right.size().height / 2);
            int rightCameraCentre_x = rightCamera_CentrePoint.x;
            int rightCameraCentre_y = rightCamera_CentrePoint.y;

            int rightCamera_dx = (targetCentreRight.x - rightCameraCentre_x);
            int rightCamera_dy = (targetCentreRight.y - rightCameraCentre_y);

            // Draw the tracking threshold square on both camera frames
            Scalar squareColour(0, 255, 0); // Green colour for the square
            int squareThickness = 2;

            // Draw square on the left camera frame
            rectangle(left,
                      Point(leftCameraCentre_x - trackingThreshold, leftCameraCentre_y - trackingThreshold),
                      Point(leftCameraCentre_x + trackingThreshold, leftCameraCentre_y + trackingThreshold),
                      squareColour, squareThickness);

            // Draw square on the right camera frame
            rectangle(right,
                      Point(rightCameraCentre_x - trackingThreshold, rightCameraCentre_y - trackingThreshold),
                      Point(rightCameraCentre_x + trackingThreshold, rightCameraCentre_y + trackingThreshold),
                      squareColour, squareThickness);

            // check if the target is outside the tracking threshold square
            bool targetOutOfThresholdRight = abs(rightCamera_dx) > trackingThreshold || abs(rightCamera_dy) > trackingThreshold;
            bool targetOutOfThresholdLeft = abs(leftCamera_dx) > trackingThreshold || abs(leftCamera_dy) > trackingThreshold;

            if(targetOutOfThresholdLeft && targetOutOfThresholdRight) {

                // normalised values
                int normaliseValue = int(3.5);

                int dx_Left = leftCamera_dx / normaliseValue;
                int dy_Left = leftCamera_dy / normaliseValue;

                int dx_Right = rightCamera_dx / normaliseValue;
                int dy_Right = rightCamera_dy / normaliseValue;

                // track the target with the left and right eye
                owl.setServoRelativePositions(dx_Right, -dy_Right, dx_Left, -dy_Left, 0);


                // calculating distance of target from left camera
                // distance calculation variables

                float leftCameraAngle, rightCameraAngle, l2, r2, distanceFromLeft, y;
                float dx_eyes = 6.7;

                owl.getServoAngles(leftCameraAngle,rightCameraAngle); // get the left and right servo angle.

                r2 = 90 - rightCameraAngle ;
                l2 = 90 - leftCameraAngle;
                y = 180 - (r2 + l2);
                    
                distanceFromLeft = dx_eyes * sin(r2) / sin(y);
                string lengthFromLeft = to_string(distanceFromLeft);
                putText(left, lengthFromLeft, leftCamera_CentrePoint + Point(-100, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
            
                cout << "r2: " << r2 << endl;
                cout << "l2: " << l2 << endl;
                cout << "y: " << y << endl << endl;
                cout << "distanceFromLeft: " << distanceFromLeft << endl;

            }
        }

        else {
            cout << "Target not found. Initiating search pattern" << endl;
            cout << "minValue left: " << minValueLeft << endl;
            cout << "minValue right: " << minValueRight << endl << endl;

            // Initiating search pattern
//            int searchRange = 300;
//            int stepSize = 10;

//            for (int n = 0; n <= searchRange; n+= stepSize)
//            {
//               owl.setServoRelativePositions(0, 0, 0, 0, stepSize);
//               owl.getCameraFrames(left, right);
//               imshow("left", left);
//               imshow("righ", right);

//               waitKey(5);
//            }

//            for(int m = 0; m <= searchRange; m+= stepSize)
//            {
//                owl.setServoRelativePositions(0, 0, 0, 0, -stepSize);
//                owl.getCameraFrames(left, right);
//                imshow("left", left);
//                imshow("right", right);
//                waitKey(5);
//            }

//            cout << "Min Value Left: " << minValueLeft << endl;
//            cout << "Min Value Right: " << minValueRight << endl;
        }

        //display camera frames
        imshow("left", left);
        imshow("right", right);
        //imshow("Matched Left", matchOutputLeft);
        //imshow("Matched Right", matchOutputRight);
        imshow("target", target);
        waitKey(5);
    }
}
