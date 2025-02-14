//James Rogers Nov 2023 (c) Plymouth University

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <iostream>
#include <string>

#include "../owl.h"
#include <opencv2/opencv.hpp> // library for colour conversion

using namespace std;
using namespace cv;

int main()
{

    //connect with the owl and load calibration values
    robotOwl owl(1500, 1475, 1520, 1525, 1520);

    while (true){
        //read the owls camera frames
        Mat left, right, FrameHSV;
        owl.getCameraFrames(left, right);


        // colour conversion
        cvtColor(left, FrameHSV, COLOR_BGR2HSV);

        Mat FrameFiltered;

        /*threshold for yellow (hue, sat, val)*/
        Vec3b LowerBound (19, 100, 100);
        Vec3b UpperBound (35, 255, 255);

        inRange(FrameHSV, LowerBound, UpperBound, FrameFiltered);

        Moments m = moments (FrameFiltered, true);

//        if (int (m.m00) == 0) { // sort this out later

//            return 0;
//        }
        Point2f p (m.m10/m.m00, m.m01/m.m00); // the Point p is the centre of mass of the colour. m.m00 can't be 0

        Scalar markerColor (0, 0, 255);
        int markerSize = 15, markerThickness = 5;
        drawMarker(FrameFiltered, p, markerColor, MARKER_CROSS, markerSize, markerThickness);

        //owl.setServoAbsolutePositions(100,0,0,0, 100);

        double px = m.m10/m.m00;
        double py = m.m01/m.m00;

        //owl.setServoRelativePositions(0, 0, int(px), int(py), 0);
        //owl.setServoAbsolutePositions(0, 0, int(px), int(py), 0);

        cout << "px: " << px << endl;
        cout << "py: "<< py << endl << endl;;

        // for debugging

        //display camera frame
        imshow("left",FrameFiltered);
        waitKey(10);
    }
}


















