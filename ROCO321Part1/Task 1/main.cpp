//James Rogers Oct 2023 (c) Plymouth University

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
    robotOwl owl(1500, 1475, 1520, 1525, 1520, true); //starts in "quiet mode" which switches off the servos.

    while (true){
        //read the owls camera frames
        Mat left, right, hsv_image; // stores images
        owl.getCameraFrames(left, right); //captures left and right camera frames

        //get pixel colour values
        Point centrePoint(left.size().width/2, left.size().height/2); // sets the size of the centrepoint
        Vec3b pixelValue = left.at<Vec3b>(centrePoint); //gets the pixel values from the centrepoint of the image stored in left cam
        unsigned char red  =pixelValue[2];
        unsigned char green=pixelValue[1];
        unsigned char blue =pixelValue[0];
        
        /*note that in openCV RGB is reversed to BGR*/

        /*conversion from RGB - HSV*/ 
        cvtColor(left, hsv_image, COLOR_BGR2HSV);

        /*defining colour thresholds*/
        Scalar lower_red (0,100,100), upper_red (20,200,200);

        Mat mask_red; // variable to store the output of the mask

        inRange(hsv_image, lower_red, upper_red, mask_red);

        /*gets the hsv colour value at the centre of the pixel hsv_image */
        Vec3b hsv_image_pixel = hsv_image.at<Vec3b> (centrePoint);
        int hue = hsv_image_pixel[0];
        int sat = hsv_image_pixel[1];
        int val = hsv_image_pixel[2];

        string textColour;
        /*RED*/
        if ((hue >= 170 && hue <= 179) || (hue >= 1 && hue <= 9)) {
            if (sat >=125 && sat <= 250) {
                textColour = "RED";
                putText(left, textColour, centrePoint+Point(200,200), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,255), 2);
            }
        }

        /*GREEN*/
        if (hue >= 50 && hue <= 65) {
            if (sat >= 125 && sat <=250) {
                textColour = "GREEN";
                putText(left, textColour, centrePoint+Point(200,200), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,255,0), 2);
            }
        }

        /*BLUE*/
        if (hue >= 105 && hue <= 125) {
            if (sat >= 125 && sat <= 250) {
                textColour = "BLUE";
                putText(left, textColour, centrePoint+Point(200,200), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2);
            }
        }

        /*YELLOW*/
        if (hue >= 25 && hue <= 35) {
            if (sat >= 125 && sat <= 250) {
                textColour = "YELLOW";
                putText(left, textColour, centrePoint+Point(200,200), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,255), 2);
            }
        }


        cout << val << endl;

        /*drawing functions*/
        circle(left, centrePoint, 15, Scalar(255,255,255), 2); //draw a circle to show the pixel being measured
        string text = "(" + to_string(red) + ", " + to_string(green) + ", " + to_string(blue) + ")";      //create a string of the RGB values (255,255,255)
        putText(left, text, centrePoint+Point(-50,100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,255,255), 2); //draw the string to the image

        /*display image*/
        imshow("left",left);
        waitKey(30);
    }
} 



















