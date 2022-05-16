// BreakBeam.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <stdio.h>
using namespace cv;
using std::vector;

vector<vector<Point>> FilterShapes(vector<vector<Point>> contours, int sizeFilter)
{
    vector<vector<Point>> approxContours = {};

    for (int i = 0; i < contours.size(); i++)
    {
        vector<Point> polygonPoints = {};
        approxPolyDP(contours[i], polygonPoints, arcLength(contours[i], true) * 0.01, true);
        if (contourArea(polygonPoints) > sizeFilter)
        {
            approxContours.push_back(polygonPoints);
        }
    }
    return approxContours;
}

int main()
{
    cv::VideoCapture videoCapture;
    Mat originalMat{};
    Mat hsvMat{};
    Mat mask{};
    Mat blur{};
    Mat canny{};

    videoCapture.open("http://192.168.1.121:1181/?action=stream");

    while (true)
    {
        while (!videoCapture.grab()) {}
        videoCapture.retrieve(originalMat);
        
        cv::cvtColor(originalMat, hsvMat, ColorConversionCodes::COLOR_BGR2HSV);
        imshow("hsv", hsvMat);
        GaussianBlur(hsvMat, blur, { 51, 51 }, 1);
        imshow("blur", blur);

        inRange(blur, Scalar(35, 100, 100), Scalar(210 / 2, 255, 255), mask);
        imshow("mask", mask);

        Canny(mask, canny, 10, 50);

        vector<vector<Point>> contours = {};
        findContours(mask, contours, RetrievalModes::RETR_LIST, CHAIN_APPROX_TC89_KCOS);

        auto approxContours = FilterShapes(contours, 700);
        drawContours(originalMat, approxContours, -1, Scalar(255, 0, 0), 2);

        if (approxContours.size() != 1)
        {
            std::cout << "Failed" << std::endl;
        }
        
        imshow("originalMat", originalMat);
        cv::waitKey(1);
    }
}



