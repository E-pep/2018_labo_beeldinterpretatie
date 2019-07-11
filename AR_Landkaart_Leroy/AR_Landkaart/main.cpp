#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


///--------------------------------------Variabelen-------------------------------------
Mat testafbeelding;
Mat testafbeelding_gray;
Mat cannyafbeelding;
Mat mapafbeelding;
Mat mapafbeelding_gray;
vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
RNG rng(12345);

int lowThreshold = 0;
const int max_lowThreshold = 100;
const int ratio = 3;
const int kernel_size = 5;
const char* window_name = "Edge Map";

///-------------------------------------- functie declaraties---------------------------------------

void detectAndDisplay(Mat frame, HOGDescriptor hog);

int main(int argc,const char** argv)
{


    ///--------------------------------------Input & Argumenten-----------------------------

    CommandLineParser parser(argc,argv,
    "{help h|  |show this message}"
    "{image  im|  |(required)}"
    "{map  mp|  |(required)}"
    );

    if(parser.has("help"))
    {
        cout << "geef volledig path van foto's mee als argument" << endl;
        parser.printMessage();
        return 0;
    }

    string image_string(parser.get<string>("image"));
    if(image_string.empty())
    {
        cout << "argument niet gevonden" << endl;
        parser.printMessage();
        return -1;
    }


    testafbeelding = imread(image_string, IMREAD_COLOR);

    if( testafbeelding.empty() )                      /// Check for invalid input
    {
        cout <<  "Could not open or find image1" << std::endl ;
        return -1;
    }



    string map_string(parser.get<string>("map"));
    if(map_string.empty())
    {
        cout << "argument niet gevonden" << endl;
        parser.printMessage();
        return -1;
    }

    mapafbeelding = imread(map_string, IMREAD_COLOR);

    if( mapafbeelding.empty() )                      /// Check for invalid input
    {
        cout <<  "Could not open or find image1" << std::endl ;
        return -1;
    }


    // videocapture webcam


    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
    {
        cout <<  "Could not open or find image1" << endl ;
        return -1;
    }



    // veranderingen op de afbeeldingen zelf zoals contour en shit!!!!!!!!!


    cvtColor( testafbeelding, testafbeelding_gray, COLOR_BGR2GRAY );
    cvtColor( mapafbeelding, mapafbeelding_gray, COLOR_BGR2GRAY );

    Canny( testafbeelding_gray, cannyafbeelding, 100, 100*2 );
    imshow( "canny afbeelding", cannyafbeelding );

    findContours(cannyafbeelding, contours,hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);


    /// Draw contours
    Mat masker = Mat::zeros(cannyafbeelding.rows, cannyafbeelding.cols, CV_8UC1);

    drawContours(masker, contours, -1, Scalar(255), CV_FILLED);
    /// Show in a window
    namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    imshow( "Contours", masker );


  // Trying with HOG descriptor
    HOGDescriptor hog;
    vector< float > descriptors;
    hog.compute(masker, descriptors);

    hog.setSVMDetector(descriptors);



    Mat edges;
    namedWindow("edges",1);
    for(;;)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
        cvtColor(frame, edges, COLOR_BGR2GRAY);
        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        Canny(edges, edges, 0, 30, 3);
        imshow("edges", edges);

        // Calculate Moments
        double d1 = matchShapes(masker, edges, CONTOURS_MATCH_I1, 0);
        cout << "momenten vergelijken:" << d1 << endl;
         detectAndDisplay(frame, hog);
        if(waitKey(30) >= 0) break;
    }


















  ///OTSU threshold voor map beter

   Mat mapafbeelding_otsu;
   threshold(mapafbeelding_gray,mapafbeelding_otsu,0,255,THRESH_OTSU | THRESH_BINARY_INV);
   imshow("OTSU",mapafbeelding_otsu);

   erode(mapafbeelding_otsu, mapafbeelding_otsu, Mat(), Point(-1,-1), 2);
   //dilate(mapafbeelding_otsu, mapafbeelding_otsu, Mat(), Point(-1,-1), 2);
imshow("opening", mapafbeelding_otsu);

    //vinden van connected components (landen)
    vector<vector<Point> > contours_map;
    findContours(mapafbeelding_otsu, contours_map,hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);



        vector< vector<Point>> hulls;

        for(size_t i=0; i<contours_map.size(); i++)
        {
        vector<Point> hull;
        convexHull(contours_map[i], hull);
        hulls.push_back(hull);
        }
     Mat mask3 = Mat::zeros(mapafbeelding_otsu.rows, mapafbeelding_otsu.cols, CV_8UC1);
    drawContours(mask3, hulls, -1, 255, -1);
    imshow("contours_map", mask3);


    waitKey(0);
    return 0;
}


void detectAndDisplay(Mat frame, HOGDescriptor hog)
{
    Mat frame_grijs;
    cvtColor(frame,frame_grijs,COLOR_BGR2GRAY);
    equalizeHist(frame_grijs, frame_grijs);


    //detect faces

    vector<Rect> found;
    vector<double> score;
    hog.detectMultiScale(frame, found,score);
    for ( size_t i = 0; i < found.size(); i++ )
    {
        // The HOG detector returns slightly larger rectangles than the real objects,
        // so we slightly shrink the rectangles to get a nicer output.

        rectangle(frame, found[i].tl(),found[i].br(), cv::Scalar(0, 255, 0), 2);
        putText(frame, to_string(score[i]),found[i].tl(), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255), 2 , 8 , false);
    }
    //-- Show what you got
    imshow( "gevonden persoon", frame );
}

