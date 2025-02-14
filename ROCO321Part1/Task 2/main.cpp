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
        Vec3b LowerBound (19, 100, 30);
        Vec3b UpperBound (35, 255, 255);

        inRange(FrameHSV, LowerBound, UpperBound, FrameFiltered);

        int hue_lower = LowerBound[0];
        int hue_upper = UpperBound[0];


        cout << hue_lower << endl;
        cout << hue_upper << endl;



        //display camera frame
        imshow("left",FrameFiltered);
        waitKey(10);
    }
}


















