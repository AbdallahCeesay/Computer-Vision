//James Rogers Nov 2023 (c) Plymouth University

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <iostream>
#include <string>

#include "../owl.h"

using namespace std;
using namespace cv;

#define targetSize 60 //adjust this to change the size of your target

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

        //your tracking code goes here
        /*matchTempllate for the left eye*/
        matchTemplate(left, target, matchOutputLeft, TM_SQDIFF_NORMED);

        double minValueLeft, maxValue;
        Point minLocationLeft, maxLocationLeft;

        minMaxLoc(matchOutputLeft, &minValueLeft, &maxValue, &minLocationLeft, &maxLocationLeft);


        /*target visibility threshold left eye*/

        double thresholdLeft = 0.2;
        Point targetCentreLeft(minLocationLeft.x + target.cols / 2, minLocationLeft.y + target.rows / 2);
        if(minValueLeft < thresholdLeft)
        {
            /*calculate the centre of the target*/


            Scalar markerColour(0, 0, 255);
            int markerSize = 15, markerThickness = 5;

            drawMarker(left, targetCentreLeft, markerColour, MARKER_CROSS, markerSize, markerThickness);
        }
        else {
            cout << "Target not found - Left eye" << endl;
        }


        //================== matchTemplate for the right eye =======================================

        matchTemplate(right, target, matchOutputRight, TM_SQDIFF_NORMED);

        double minValueRight, maxValueRight;
        Point minLocationRight, maxLocationRight;

        minMaxLoc(matchOutputRight, &minValueRight, &maxValueRight, &minLocationRight, &maxLocationRight);

        //target visibility threshold right eye*/
        double thresholdRight = 0.2;
        if(minValueRight < thresholdRight)
        {
            /*calculate the centre of the target*/
            Point targetCentreRight(minLocationRight.x + target.cols / 2, minLocationRight.y + target.rows / 2);

            Scalar markerColour(0, 0, 255);
            int markerSize = 15, markerThickness = 5;

            drawMarker(right, targetCentreRight, markerColour, MARKER_CROSS, markerSize, markerThickness);
        }
        else {
            cout << "Target not found - Right eye" << endl;
        }

        //===================== Moving eyes to track the target ========================================


        /*check if the target is visible in both camera frames*/
        if(minValueLeft < thresholdLeft || minValueRight < thresholdRight) {

            Point leftCamera_CentreFrame(left.size().width / 2, left.size().height / 2);
            int leftCamera_CentreFrame_x = leftCamera_CentreFrame.x;
            int leftCamera_CentreFrame_y = leftCamera_CentreFrame.y;

            int leftCameraFrame_dx = (leftCamera_CentreFrame_x - targetCentreLeft.x);
            int leftCameraFrame_dy = (leftCamera_CentreFrame_y - targetCentreLeft.y);

            // track the target with the left eye
            owl.setServoRelativePositions(0, 0, minLocationLeft.x, minLocationLeft.y, 0);

//            cout << "leftCamera_CentreFrame_x: " << leftCamera_CentreFrame_x << endl;
//            cout << "leftCamera_CentreFrame_y: " << leftCamera_CentreFrame_y << endl << endl;

//            cout << "minLocationleft: " << minLocationLeft << endl;

            cout << "leftCameraFrame_dx: " << leftCameraFrame_dx << endl;
            cout << "leftCameraFrame_dy: " << leftCameraFrame_dy << endl;


        }
        else {
            // centre the cameras or do a sweep to get if the target in visible. do this a set number of times and if the target is still
            //not visilble centre the camera frames
        }


//        cout << "MinLocation left" << minLocationLeft << endl;
//        cout << "MinLocation right" << minLocationRight << endl << endl;



        //display camera frames
        imshow("left", left);
        imshow("right", right);
        imshow("target", target);
        imshow("matchOutputLeft", matchOutputLeft);
        imshow("matchOutputRight", matchOutputRight);
        waitKey(10);
    }


}


















