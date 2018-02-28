
#pragma once

#include "resource.h"
#include <cv.h>
#include "opencv2\calib3d\calib3d.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\video\video.hpp"

using namespace cv;
using namespace std;

class CameraCalibration
{
    static const int        cColorWidth  = 640;
    static const int        cColorHeight = 480;

    static const int        cStatusMessageMaxLen = MAX_PATH*2;

	char s[100];
	Mat IR_gray, IR_gray_image, IR_gray_image_;

	vector<vector<Point3f>> IR_object_points;
	vector<vector<Point2f>> IR_image_points;
	vector<vector<Point3f>> color_object_points;
	vector<vector<Point2f>> color_image_points;

	vector<Point2f> IR_corners;
	vector<Point2f> color_corners;

	vector<Mat> IR_rvecs;
	vector<Mat> IR_tvecs;
	vector<Mat> color_rvecs;
	vector<Mat> color_tvecs;

	char IR_res_img[100];





public:
    
    // Constructor
    CameraCalibration();

    // Destructor
    ~CameraCalibration();

    // Handles window messages, passes most to the class instance to handle
    static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    // Handle windows messages for a class instance
    LRESULT CALLBACK        DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    // Creates the main window and begins processing
    int                     Run(HINSTANCE hInstance, int nCmdShow);


	// getCounter  - return the value of camera counter
	int						getCameraCounter();
		
	// setCounter  - sets the value of camera counter
	void					setCameraCounter(int );
		


private:


	int						newCameraCount;
    HWND                    m_hWnd;	
	bool                    m_bCameraNumberReceived;

 
    // Handle new color data
    void                    ProcessColor();

    // Set the status bar message
    void                    SetStatusMessage2(WCHAR* szMessage);
	void                    SetStatusMessage1(WCHAR* szMessage2);

	void                    SetStatusMessage4(WCHAR* szMessage4);
    void                    SetStatusMessage5(WCHAR* szMessage5);
	void                    SetStatusMessage6(WCHAR* szMessage6);
	void                    SetStatusMessage7(WCHAR* szMessage7);
	void                    SetStatusMessage8(WCHAR* szMessage8);

};
