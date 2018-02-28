#include "stdafx.h"
#include <strsafe.h>
#include "CameraCalibration.h"
#include <string>
#include "resource.h"
#include <cv.h>
#include "opencv2\calib3d\calib3d.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"

#define IR 0 // IR or Color
using namespace cv;
using namespace std;

string pathname = "C:\\CubiScan";


Size patternsize(10,6);

// Entry point for the application
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    CameraCalibration application;
    application.Run(hInstance, nCmdShow);
}

// Constructor
CameraCalibration::CameraCalibration() :

    m_bCameraNumberReceived(false),
	newCameraCount(0) // camera number   

{
}

// Destructor
CameraCalibration::~CameraCalibration()
{
 
}

// Creates the main window and begins processing
int CameraCalibration::Run(HINSTANCE hInstance, int nCmdShow)
{
    MSG       msg = {0};
    WNDCLASS  wc;

    // Dialog custom window class
    ZeroMemory(&wc, sizeof(wc));
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.cbWndExtra    = DLGWINDOWEXTRA;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_APP));
    wc.lpfnWndProc   = DefDlgProcW;
    wc.lpszClassName = L"CameraCalibrationAppDlgWndClass";

    if (!RegisterClassW(&wc))
    {
        return 0;
    }

    // Create main application window
    HWND hWndApp = CreateDialogParamW(hInstance, MAKEINTRESOURCE(IDD_APP), NULL, (DLGPROC)CameraCalibration::MessageRouter, reinterpret_cast<LPARAM>(this));

    // Show window
    ShowWindow(hWndApp, nCmdShow);

    const int eventCount = 1;
   // HANDLE hEvents[eventCount];

    // Main message loop
    while (WM_QUIT != msg.message)
    {
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // If a dialog message will be taken care of by the dialog proc
            if ((hWndApp != NULL) && IsDialogMessageW(hWndApp, &msg))
            {
                continue;
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    return static_cast<int>(msg.wParam);
}



// Handles window messages, passes most to the class instance to handle
LRESULT CALLBACK CameraCalibration::MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CameraCalibration* pThis = NULL;
    
    if (WM_INITDIALOG == uMsg)
    {
        pThis = reinterpret_cast<CameraCalibration*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        pThis = reinterpret_cast<CameraCalibration*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pThis)
    {
        return pThis->DlgProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

// Handle windows messages for the class instance
LRESULT CALLBACK CameraCalibration::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	

    switch (message)
    {
        case WM_INITDIALOG:
        {
            // Bind application window handle
            m_hWnd = hWnd;
        }
        break;

        // If the titlebar X is clicked, destroy app
        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;

        case WM_DESTROY:
            // Quit the main message pump
            PostQuitMessage(0);
            break;

        // Handle button press
        case WM_COMMAND:

            // If it was for the camera control and a button clicked event, save 4 sets of images 
            if (IDOK == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
            {
                
				if (m_bCameraNumberReceived == false)
				{
					//
					// Initialize parameters for the user input
					//
					HWND hWndCameraNumber;
					int camera_number = 0;
					TCHAR strcamera_number[10];
				
					hWndCameraNumber = GetDlgItem(hWnd, IDC_INPUTBOX_DLG_EDIT1);
					
					//
					// Retrieve the camera number
					//
					GetWindowText(hWndCameraNumber, LPWSTR(strcamera_number), 80);
					camera_number =  _wtoi(strcamera_number);


					if (camera_number !=0)
					{
						//
						// Set camera number (use for display purpose)
						//
						setCameraCounter(camera_number);
						

						m_bCameraNumberReceived = true;
						ProcessColor();
					}
				}

            }
            break;
    }

    return FALSE;
}




// Handle new color data
void CameraCalibration::ProcessColor()
{   
	
	//HRESULT hr;
  
	int number_of_images = 40;
    int cameraNumber  = getCameraCounter();
	Mat IR_intrinsic,IR_distCoeffs;
	char strset[10],strImage[10];

	WCHAR statusMessage2[cStatusMessageMaxLen];
	WCHAR statusMessage4[cStatusMessageMaxLen];

		//If the user pressed the screenshot button, save a screenshot            
		if (m_bCameraNumberReceived == true )
		{
		
			SetStatusMessage1(L"Reading images and Comp. Cam. Cali.");
			for (int setNumber = 1; setNumber<= 4; setNumber ++ ) 
			{
				
				if (setNumber == 1)
					SetStatusMessage5(L"Reading Images");			
				else if (setNumber == 2)
					SetStatusMessage6(L"Reading Images");
				else if (setNumber == 3)
					SetStatusMessage7(L"Reading Images");
				else
					SetStatusMessage8(L"Reading Images");
				
				
				
				
				for ( int index=0; index< number_of_images;index = index+1)			
				{				
						// Reading IR images
						sprintf(s,"\\Camera_IR-%d\\IR-%d\\IR_images_-%d.bmp",cameraNumber, setNumber,index);
						IR_gray=imread(pathname+s,1);

						// Check for invalid input
						if( !IR_gray.data  )                              
						{
							SetStatusMessage1(L"Could not open or find the image");
						}
						else
						{
							imshow("Calibration_IR",IR_gray);
							waitKey(1);

							//IR image checker board pattern
							cvtColor(IR_gray,IR_gray_image,CV_BGR2GRAY); 
			    
							//check if the pattern found
							bool IR_patternfound   =findChessboardCorners(IR_gray_image,patternsize,IR_corners,CALIB_CB_ADAPTIVE_THRESH +CV_CALIB_CB_NORMALIZE_IMAGE    );//CALIB_CB_FAST_CHECK
			
							// Find the points if and only if IR and Color pattern found
							if(IR_patternfound )
							{

								cornerSubPix(IR_gray_image,IR_corners,Size(11,11),Size(-1,-1),TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,100,0.1));		 
								// Gray to RGB
								cvtColor(IR_gray_image,IR_gray_image,CV_GRAY2BGR);
								drawChessboardCorners(IR_gray_image,patternsize,Mat(IR_corners),IR_patternfound);
								resize(IR_gray_image, IR_gray_image_, Point(1280,960), 0, 0, INTER_CUBIC );
								sprintf(strset,  "Set  - %d",setNumber);
								putText(IR_gray_image_, strset, cvPoint(30,30), 
															  FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 255, 0), 2, CV_AA );

								sprintf(strImage,"Image - %d",index);
								putText(IR_gray_image, strImage, cvPoint(30,80), 
															  FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 255, 0), 2, CV_AA );
								imshow( "IR with pattern", IR_gray_image );
								sprintf(s,"\\Camera_IR-%d\\IR-%d\\IR_images_-%d.jpg",cameraNumber, setNumber,index);
								imwrite(pathname + s ,IR_gray_image);
								waitKey(1);							
								
								
								vector<Point3f> obj;
								
								for(int j=0;j<60;j++)
								{  
									obj.push_back(Point3f(j/10, j%10, 0.0f));	
								}

								 
								IR_object_points.push_back(obj);
								IR_image_points.push_back(IR_corners);
							}
						}
						// Status: Display Camera number 
						if (cameraNumber <= 9)
							StringCchPrintf( statusMessage2, cStatusMessageMaxLen, L"Camera #   %6d", cameraNumber);
						else if (cameraNumber <= 99)
							StringCchPrintf( statusMessage2, cStatusMessageMaxLen, L"Camera #   %5d", cameraNumber);
						else
							StringCchPrintf( statusMessage2, cStatusMessageMaxLen, L"Camera #   %4d", cameraNumber);
						SetStatusMessage2(statusMessage2);

						

						// Status: Display image number
						if (index <= 9)
							StringCchPrintf( statusMessage4, cStatusMessageMaxLen, L" Frame #   %6d", index);
						else if (index <= 99)
							StringCchPrintf( statusMessage4, cStatusMessageMaxLen, L" Frame #   %5d", index);
						else
							StringCchPrintf( statusMessage4, cStatusMessageMaxLen, L" Frame #   %4d", index);
				
						SetStatusMessage4(statusMessage4);
						
				}
				if (IR_object_points.size() >=7) 
				{

					if (setNumber == 1)
						SetStatusMessage5(L"Calibrating !!!!");						
					else if (setNumber == 2)
						SetStatusMessage6(L"Calibrating !!!!");
					else if (setNumber == 3)
						SetStatusMessage7(L"Calibrating !!!!");
					else
						SetStatusMessage8(L"Calibrating !!!!");

		
					double rms_IR = calibrateCamera(IR_object_points, IR_image_points,  IR_gray.size(), IR_intrinsic,  IR_distCoeffs, IR_rvecs, IR_tvecs);
			
					sprintf(s,"\\Camera_IR-%d\\IR camera-%d.txt",cameraNumber, setNumber);
					FileStorage fs_IR( pathname+s, FileStorage::WRITE);
					fs_IR<<"Intrinsic Parameter "<<IR_intrinsic;
					fs_IR<<"Distortion parameter "<<IR_distCoeffs;
					fs_IR<<"Rms_IR "<<rms_IR;

				
					if (setNumber == 1)
						SetStatusMessage5(L"Done !!!!");						
					else if (setNumber == 2)
						SetStatusMessage6(L"Done !!!!");
					else if (setNumber == 3)
						SetStatusMessage7(L"Done !!!!");
					else
						SetStatusMessage8(L"Done !!!!");
           
					
            
				}
				else
				{
					if (setNumber == 1)
						SetStatusMessage5(L"Not Suff. Images.");
					else if (setNumber == 2)
						SetStatusMessage6(L"Not Suff. Images.");
					else if (setNumber == 3)
						SetStatusMessage7(L"Not Suff. Images.");
					else
						SetStatusMessage8(L"Not Suff. Images.");
				}					
			}
			m_bCameraNumberReceived = false;				
		}	
}



//
// Set the status bar message
//
void CameraCalibration::SetStatusMessage1(WCHAR * szMessage)
{
    SendDlgItemMessageW(m_hWnd, IDC_STATUS1, WM_SETTEXT, 0, (LPARAM)szMessage);
	
}
void CameraCalibration::SetStatusMessage2(WCHAR * szMessage2)
{
   
	SendDlgItemMessageW(m_hWnd, IDC_STATUS2, WM_SETTEXT, 0, (LPARAM)szMessage2);
}

void CameraCalibration::SetStatusMessage4(WCHAR * szMessage4)
{
   
	SendDlgItemMessageW(m_hWnd, IDC_STATUS4, WM_SETTEXT, 0, (LPARAM)szMessage4);
}
void CameraCalibration::SetStatusMessage5(WCHAR * szMessage5)
{
   
	SendDlgItemMessageW(m_hWnd, IDC_STATUS5, WM_SETTEXT, 0, (LPARAM)szMessage5);
}
void CameraCalibration::SetStatusMessage6(WCHAR * szMessage6)
{
   
	SendDlgItemMessageW(m_hWnd, IDC_STATUS6, WM_SETTEXT, 0, (LPARAM)szMessage6);
}

void CameraCalibration::SetStatusMessage7(WCHAR * szMessage7)
{
   
	SendDlgItemMessageW(m_hWnd, IDC_STATUS7, WM_SETTEXT, 0, (LPARAM)szMessage7);
}

void CameraCalibration::SetStatusMessage8(WCHAR * szMessage8)
{
   
	SendDlgItemMessageW(m_hWnd, IDC_STATUS8, WM_SETTEXT, 0, (LPARAM)szMessage8);
}


//
//camera counter
//
int CameraCalibration::getCameraCounter(){
	return newCameraCount;
}
void CameraCalibration::setCameraCounter(int setCameraCount){
	newCameraCount  = setCameraCount;
}
