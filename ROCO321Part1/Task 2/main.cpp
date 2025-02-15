﻿//James Rogers Nov 2023 (c) Plymouth University

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <iostream>
#include <string>

#include "../owl.h"
#include <opencv2/opencv.hpp> // library for colour conversion

using namespace std;
using namespace cv;
const int normalised_value = 5;

int main()
{

    //connect with the owl and load calibration values
    robotOwl owl(1500, 1475, 1520, 1525, 1520);

    owl.setServoAbsolutePositions(0,0,0,0,0); // Resets servos to default positon

    while (true)
    {
        Mat left, right, FrameHSV;
        owl.getCameraFrames(left, right);

        Point2f centreFrame (left.size().width/2, left.size().height/2);

        int centreFrame_x = int(centreFrame.x);
        int centreFrame_y = int(centreFrame.y);


        cvtColor(left, FrameHSV, COLOR_BGR2HSV); // colour conversion


        /*threshold for yellow (hue, sat, val)*/
        Vec3b LowerBound (50, 100, 100);
        Vec3b UpperBound (76, 255, 255);

        Mat FrameFiltered;
        inRange(FrameHSV, LowerBound, UpperBound, FrameFiltered);

        Moments m = moments(FrameFiltered, true);

        if (int(m.m00) == 0) { //division by zero error

            cout << "divison by zero error" << endl;
            owl.setServoRelativePositions(0,0,0,0,0);

            continue;
        }

        Point2f colourCentreMass (float(m.m10 / m.m00), float(m.m01 / m.m00)); // the Point p is the centre of mass of the colour. m.m00 can't be 0

        Scalar markerColor (0, 0, 255);
        int markerSize = 15, markerThickness = 5;
        drawMarker(left, colourCentreMass, markerColor, MARKER_CROSS, markerSize, markerThickness);


        int px = int(colourCentreMass.x);
        int py = int(colourCentreMass.y);

        int dx = (px - centreFrame_x) /normalised_value;
        int dy = -(py - centreFrame_y) /normalised_value;


        owl.setServoRelativePositions(0, 0, dx, dy, 0);

        // for debugging

        cout << "px: " << px << endl;
        cout << "py: "<< py << endl << endl;
        cout << "dx: " << dx << endl;
        cout << "dy: " << dy << endl << endl;

        //display camera frame
        imshow("left",left);
        waitKey(10);
    }
}


















