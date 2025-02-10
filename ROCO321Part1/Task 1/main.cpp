//James Rogers Oct 2023 (c) Plymouth University

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <iostream>
#include <string>

#include "../owl.h"

using namespace std;
using namespace cv;

int main()
{
    //connect with the owl and load calibration values
    robotOwl owl(1500, 1475, 1520, 1525, 1520, true); //starts in "quiet mode" which switches off the servos.

    while (true){
        //read the owls camera frames
        Mat left, right; // stores images
        owl.getCameraFrames(left, right); //captures left and right camera frames

        //get pixel colour values
        Point centrePoint(left.size().width/2, left.size().height/2); // sets the size of the centre point
        Vec3b pixelValue = left.at<Vec3b>(centrePoint); //gets the pixel values from the centre point
        unsigned char red  =pixelValue[2];
        unsigned char green=pixelValue[1];
        unsigned char blue =pixelValue[0];


        if (red >= 235) {
            string red_text = "red";
            putText(left,red_text, centrePoint+Point(200,200),FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
        }


        if (green >= 235) {
            string red_text = "green";
            putText(left,red_text, centrePoint+Point(200,200),FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 225, 0), 2);
        }


        if (blue >= 235) {
            string red_text = "blue";
            putText(left,red_text, centrePoint+Point(200,200),FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 2);
        }



        //drawing functions
        circle(left, centrePoint, 15, Scalar(255,255,255), 2); //draw a circle to show the pixel being measured
        string text = "(" + to_string(red) + ", " + to_string(green) + ", " + to_string(blue) + ")";      //create a string of the RGB values (255,255,255)
        putText(left, text, centrePoint+Point(-50,100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,255,255), 2); //draw the string to the image

        //display image
        imshow("left",left);
        waitKey(30);
    }
}


















