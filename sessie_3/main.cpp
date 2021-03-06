/*
* help: https://docs.opencv.org/3.4.0/de/da9/tutorial_template_matching.html
*
*/


#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void rotate(Mat& src, double angle, Mat& dst)
{
    Point2f pt(src.cols/2., src.rows/2.);
    Mat r = getRotationMatrix2D(pt, angle, 1.0);
    warpAffine(src, dst, r, cv::Size(src.cols, src.rows));
}

int main(int argc,const char** argv)
{
    ///variabless
    Mat Image1;
    Mat Image_template;
    Mat Image_result;
    Mat Image_rotated;

    ///Adding a little help option and command line parser input
    CommandLineParser parser(argc,argv,
        "{help h|  |show this message}"
        "{image_1  im1|  |(required)}"
        "{image_template  im_temp|  |(required)}"
        "{image_rotated  im2|  |(required)}"
     //   "{image_4  im4|  |(required)}"
    );

    if(parser.has("help"))
    {
        cout << "geef volledig path van foto's mee als argument" << endl;
        parser.printMessage();
        return 0;
    }


    ///Collect data from arguments

    string image_1(parser.get<string>("image_1"));
    if(image_1.empty())
    {
        cout << "argument niet gevonden" << endl;
        parser.printMessage();
        return -1;
    }

    string image_2(parser.get<string>("image_template"));
    if(image_2.empty())
    {
        cout << "argument niet gevonden" << endl;
        parser.printMessage();
        return -1;
    }

    string image_3(parser.get<string>("image_rotated"));
    if(image_2.empty())
    {
        cout << "argument niet gevonden" << endl;
        parser.printMessage();
        return -1;
    }




    ///inlezen van afbeeldingen

   Image1 = imread(image_1, IMREAD_COLOR);
   Image_template = imread(image_2, IMREAD_COLOR);
   Image_rotated = imread(image_3, IMREAD_COLOR);

    if( Image1.empty() )                      /// Check for invalid input
    {
        cout <<  "Could not open or find image1" << std::endl ;
        return -1;
    }


    ///openen van originele afbeeldingen

    //imshow("loopband", Image1);
    //imshow("template", Image_template);



    ///opdracht 1:Gebruik template matching om een object te vinden in een inputbeeld

    ///template matching toepassen zonder masker

    matchTemplate( Image1.clone(), Image_template, Image_result, TM_CCOEFF_NORMED);

    ///normaliseren van resultaat

    normalize( Image_result, Image_result, 0, 1, NORM_MINMAX, -1, Mat() );


    ///waarde maxima nemen, geeft minimum terug
    Image_result = 1- Image_result;


    ///minima en maxima localiseren
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;
    minMaxLoc( Image_result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

    matchLoc = minLoc;

    Mat img_display = Image1.clone();
    /// rechthoek tekenen rond gevonden matches
    rectangle( img_display, matchLoc, Point( matchLoc.x + Image_template.cols , matchLoc.y + Image_template.rows ), Scalar::all(0), 2, 8, 0 );
    imshow( "image", img_display );
    imshow( "result", Image_result );




    ///opdracht 2: Pas de template matching aan om lokaal naar maxima te zoeken, zodanig dat je alle matches vind





    //template matching

    Mat Image_result2;


    // originele afbeelding om op te tekenen

    Mat finaal2;
    Image1.copyTo(finaal2);

    matchTemplate( Image1, Image_template, Image_result2, TM_CCORR_NORMED);

    //normaliseren van resultaat

    normalize( Image_result2, Image_result2, 0, 1, NORM_MINMAX, -1, Mat());

    /// threshold binary inverted

    threshold(Image_result2,Image_result2,0.8,1,1);

    ///waarde maxima nemen, SQDIFF geeft minimum terug
    Image_result2 = 1- Image_result2;

    Mat img_display2 = Image1.clone();



    ///omzetten naar CV_8UC1 voor findcontours functie *255 om 1 om te zetten naar 255

    Image_result2.convertTo(Image_result2,CV_8UC1);
    Image_result2 = Image_result2*255;
    imshow( "result2", Image_result2 );


    ///contours en hulls


    vector< vector<Point>> contours;
    findContours(Image_result2.clone(), contours,  RETR_EXTERNAL, CHAIN_APPROX_NONE);
    vector< vector<Point>> hulls;

    /// rond elk gevonden match een rechthoek tekenen
    for(size_t i=0; i<contours.size(); i++)
    {
        Rect region = boundingRect(contours[i]);
        Mat temp = Image_result2(region);
        imshow("temp",temp);
        ///minima en maxima localiseren
        minMaxLoc( temp, NULL, NULL, NULL, &maxLoc);
        rectangle(finaal2,Point(region.x + maxLoc.x, region.y + maxLoc.y), Point(maxLoc.x +region.x + Image_template.cols, region.y + maxLoc.y + Image_template.rows), Scalar::all(0), 2, 8, 0 );
    }



    imshow( "image2", img_display2 );
    imshow("finaal2",finaal2);




    ///Extra: Pas de template matching aan geroteerde objecten te vinden (roteren van beeld, rotatedRect, warpPerspective op hoekpunten)
/*
    vector<Mat> rotated_images;
    Mat rotated;

    // originele afbeelding om op te tekenen

    Mat finaal3;
    Image_rotated.copyTo(finaal3);


    // vector maken gevuld met originele afbeelding geroteerd
    for(int i = 0; i < 360; i++)
    {
	rotated = Image_rotated.clone();
	cvtColor(rotated, rotated, CV_BGR2GRAY);
	equalizeHist( rotated, rotated );
    rotate(Image_rotated, (i+1), rotated);
	rotated_images.push_back(rotated);
    }

    vector< vector<Rect> > detections;

    for (int i = 0; i < (int)rotated_images.size(); i++){
        vector<Rect> objects;
        detections.push_back(objects);
    }
*/
    waitKey(0);

    return 0;
}



