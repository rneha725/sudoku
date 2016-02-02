#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"
#include <iostream>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
// #include "digitrecognizer.h"
// #include "digitrecognizer.cpp"
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
using namespace std;

typedef unsigned char BYTE;
#include <cv.h>
#include <highgui.h>

#include <ml.h>

using namespace cv;
#define MAX_NUM_IMAGES    60000
class DigitRecognizer
{
public:
    DigitRecognizer();

    ~DigitRecognizer();

    bool train(char* trainPath, char* labelsPath);

    int classify(Mat img);

private:
    Mat preprocessImage(Mat img);

    int readFlippedInteger(FILE *fp);

private:
    KNearest    *knn;
    int numRows, numCols, numImages;

};


void mergeRelatedLines(vector<Vec2f> *lines,Mat &img)
{
	vector<Vec2f>::iterator current;
	for(current=lines->begin();current!=lines->end();current++)
	{
		//rho=0 and theta=-100; impossible value for fused 
		//lines; not to be refused.
		if((*current)[0]==0&&(*current)[1]==-100) continue;
		float p1=(*current)[0];
		float theta1=(*current)[1];
		//With these two values, 
		//we find two points on the line
		Point pt1current,pt2current;
		if(theta1>CV_PI*45/180 && theta1<CV_PI*135/180)
		{
			pt1current.x=0;
			pt1current.y=p1/sin(theta1);
			pt2current.x=img.size().width;
			pt2current.y=pt2current.x/tan(theta1)+p1/sin(theta1);
		}
		else 
		{
			pt1current.y=0;

            pt1current.x=p1/cos(theta1);

            pt2current.y=img.size().height;
            pt2current.x=-pt2current.y/tan(theta1) + p1/cos(theta1);
		}
		// Next, we start iterating over the lines again
		vector<Vec2f>::iterator pos;
		for(pos=lines->begin();pos!=lines->end();pos++)
		{
			if(*current==*pos) continue;
			if(fabs((*pos)[0]-(*current)[0])<20 && fabs((*pos)[1]-(*current)[1])<CV_PI*10/180)
			{
				float p=(*pos)[0];
				float theta=(*pos)[1];
				Point pt1, pt2;
                if((*pos)[1]>CV_PI*45/180 && (*pos)[1]<CV_PI*135/180)
                {
                    pt1.x=0;
                    pt1.y = p/sin(theta);
                    pt2.x=img.size().width;
                    pt2.y=-pt2.x/tan(theta) + p/sin(theta);
                }
                else
                {
                    pt1.y=0;
                    pt1.x=p/cos(theta);
                    pt2.y=img.size().height;
                    pt2.x=-pt2.y/tan(theta) + p/cos(theta);
                }
                if(((double)(pt1.x-pt1current.x)*(pt1.x-pt1current.x) + (pt1.y-pt1current.y)*(pt1.y-pt1current.y)<64*64) &&
((double)(pt2.x-pt2current.x)*(pt2.x-pt2current.x) + (pt2.y-pt2current.y)*(pt2.y-pt2current.y)<64*64))
                {
                    // Merge the two
                    (*current)[0] = ((*current)[0]+(*pos)[0])/2;

                    (*current)[1] = ((*current)[1]+(*pos)[1])/2;

                    (*pos)[0]=0;
                    (*pos)[1]=-100;
                }
			}
		}
	}
}

void drawLine(Vec2f line, Mat &img, Scalar rgb = CV_RGB(0,0,255))
{
    if(line[1]!=0)
    {
        float m = -1/tan(line[1]);

        float c = line[0]/sin(line[1]);

        cv::line(img, Point(0, c), Point(img.size().width, m*img.size().width+c), rgb);
    }
    else
    {
        cv::line(img, Point(line[0], 0), Point(line[0], img.size().height), rgb);
    }

}

int main(int argc, char const *argv[])
{
	Mat sudoku=imread("sudoku1.jpg",0);
	Mat outerBox=Mat(sudoku.size(),CV_8UC1);
	// Blur the image a little.
	// This smooths out the noise a bit
	// and makes extracting the grid lines
	//easier.
	GaussianBlur(sudoku,sudoku,Size(11,11),0);

	adaptiveThreshold(sudoku,outerBox,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,5,2);
	bitwise_not(outerBox,outerBox);

	/*This thresholding operation can disconnect
	certain connected parts (like lines). 
	So dilating the image once will fill up 
	any small "cracks" that might have crept
	in.*/
	Mat kernel=(Mat_<uchar>(3,3)<<0,1,0,1,1,1,0,1,0);
	dilate(outerBox,outerBox,kernel);
	// namedWindow("outerBox");
	// imshow("outerBox",outerBox);


	//Finding blob of sudoku
	int count=0;
	int max=-1;
	Point maxPt;

	for(int y=0;y<outerBox.size().height;y++)
	{
		uchar *row=outerBox.ptr(y);
		for(int x=0;x<outerBox.size().width;x++)
		{
			if(row[x]>=128)
			{
				int area;
				area=floodFill(outerBox,Point(x,y),CV_RGB(0,0,64));
				if(area>max)
				{
					maxPt=Point(x,y);
					max=area;
				}
			}
		}
	}
	floodFill(outerBox, maxPt, CV_RGB(255,255,255));
	cout<<outerBox.size().width<<" "<<outerBox.size().height<<endl;
	for(int y=0;y<outerBox.size().height;y++)
	{
		uchar *row=outerBox.ptr(y);
		for(int x=0;x<outerBox.size().width;x++)
		{
			if(row[x]==64 && x!=maxPt.x && y!=maxPt.y)
			{
				cout<<"here\n";
				int area=floodFill(outerBox,Point(x,y),CV_RGB(0,0,0));
			}
		}
	}

	/*Because we had dilated the image earlier,
	 we'll "restore" it a bit by eroding it:*/

	erode(outerBox,outerBox,kernel);
	

	vector<Vec2f> lines;
	HoughLines(outerBox,lines,1,CV_PI/180,200);
	/*For now, we'll draw each line. Just to 
	see if the results too now are good enough 
	or not:*/
	for(int i=0;i<lines.size();i++)
	{
		drawLine(lines[i],outerBox,CV_RGB(0,0,128));
	}

	HoughLines(outerBox, lines, 1, CV_PI/180, 200);

    mergeRelatedLines(&lines, sudoku); // Add this line

    // Now detect the lines on extremes
    Vec2f topEdge = Vec2f(1000,1000);    double topYIntercept=100000, topXIntercept=0;
    Vec2f bottomEdge = Vec2f(-1000,-1000);        double bottomYIntercept=0, bottomXIntercept=0;
    Vec2f leftEdge = Vec2f(1000,1000);    double leftXIntercept=100000, leftYIntercept=0;
    Vec2f rightEdge = Vec2f(-1000,-1000);        double rightXIntercept=0, rightYIntercept=0;


    for(int i=0;i<lines.size();i++)
    {

        Vec2f current = lines[i];

        float p=current[0];

        float theta=current[1];

        if(p==0 && theta==-100)
            continue;
		double xIntercept, yIntercept;
        xIntercept = p/cos(theta);
        yIntercept = p/(cos(theta)*sin(theta));
        if(theta>CV_PI*80/180 && theta<CV_PI*100/180)
        {
            if(p<topEdge[0])

                topEdge = current;

            if(p>bottomEdge[0])
                bottomEdge = current;
        }
        else if(theta<CV_PI*10/180 || theta>CV_PI*170/180)
        {
            if(xIntercept>rightXIntercept)
            {
                rightEdge = current;
                rightXIntercept = xIntercept;
            }
            else if(xIntercept<=leftXIntercept)
            {
                leftEdge = current;
                leftXIntercept = xIntercept;
            }
        }
    }
    drawLine(topEdge, sudoku, CV_RGB(0,0,0));
    drawLine(bottomEdge, sudoku, CV_RGB(0,0,0));
    drawLine(leftEdge, sudoku, CV_RGB(0,0,0));
    drawLine(rightEdge, sudoku, CV_RGB(0,0,0));
    Point left1, left2, right1, right2, bottom1, bottom2, top1, top2;

    int height=outerBox.size().height;

    int width=outerBox.size().width;

    if(leftEdge[1]!=0)
    {
        left1.x=0;        left1.y=leftEdge[0]/sin(leftEdge[1]);
        left2.x=width;    left2.y=-left2.x/tan(leftEdge[1]) + left1.y;
    }
    else
    {
        left1.y=0;        left1.x=leftEdge[0]/cos(leftEdge[1]);
        left2.y=height;    left2.x=left1.x - height*tan(leftEdge[1]);

    }

    if(rightEdge[1]!=0)
    {
        right1.x=0;        right1.y=rightEdge[0]/sin(rightEdge[1]);
        right2.x=width;    right2.y=-right2.x/tan(rightEdge[1]) + right1.y;
    }
    else
    {
        right1.y=0;        right1.x=rightEdge[0]/cos(rightEdge[1]);
        right2.y=height;    right2.x=right1.x - height*tan(rightEdge[1]);

    }

    bottom1.x=0;    bottom1.y=bottomEdge[0]/sin(bottomEdge[1]);

    bottom2.x=width;bottom2.y=-bottom2.x/tan(bottomEdge[1]) + bottom1.y;

    top1.x=0;        top1.y=topEdge[0]/sin(topEdge[1]);
    top2.x=width;    top2.y=-top2.x/tan(topEdge[1]) + top1.y;


     // Next, we find the intersection of  these four lines
    double leftA = left2.y-left1.y;
    double leftB = left1.x-left2.x;

    double leftC = leftA*left1.x + leftB*left1.y;

    double rightA = right2.y-right1.y;
    double rightB = right1.x-right2.x;

    double rightC = rightA*right1.x + rightB*right1.y;

    double topA = top2.y-top1.y;
    double topB = top1.x-top2.x;

    double topC = topA*top1.x + topB*top1.y;

    double bottomA = bottom2.y-bottom1.y;
    double bottomB = bottom1.x-bottom2.x;

    double bottomC = bottomA*bottom1.x + bottomB*bottom1.y;

    // Intersection of left and top
    double detTopLeft = leftA*topB - leftB*topA;

    CvPoint ptTopLeft = cvPoint((topB*leftC - leftB*topC)/detTopLeft, (leftA*topC - topA*leftC)/detTopLeft);

    // Intersection of top and right
    double detTopRight = rightA*topB - rightB*topA;

    CvPoint ptTopRight = cvPoint((topB*rightC-rightB*topC)/detTopRight, (rightA*topC-topA*rightC)/detTopRight);

    // Intersection of right and bottom
    double detBottomRight = rightA*bottomB - rightB*bottomA;
    CvPoint ptBottomRight = cvPoint((bottomB*rightC-rightB*bottomC)/detBottomRight, (rightA*bottomC-bottomA*rightC)/detBottomRight);// Intersection of bottom and left
    double detBottomLeft = leftA*bottomB-leftB*bottomA;
    CvPoint ptBottomLeft = cvPoint((bottomB*leftC-leftB*bottomC)/detBottomLeft, (leftA*bottomC-bottomA*leftC)/detBottomLeft);
	int maxLength = (ptBottomLeft.x-ptBottomRight.x)*(ptBottomLeft.x-ptBottomRight.x) + (ptBottomLeft.y-ptBottomRight.y)*(ptBottomLeft.y-ptBottomRight.y);
    int temp = (ptTopRight.x-ptBottomRight.x)*(ptTopRight.x-ptBottomRight.x) + (ptTopRight.y-ptBottomRight.y)*(ptTopRight.y-ptBottomRight.y);

    if(temp>maxLength) maxLength = temp;

    temp = (ptTopRight.x-ptTopLeft.x)*(ptTopRight.x-ptTopLeft.x) + (ptTopRight.y-ptTopLeft.y)*(ptTopRight.y-ptTopLeft.y);

    if(temp>maxLength) maxLength = temp;

    temp = (ptBottomLeft.x-ptTopLeft.x)*(ptBottomLeft.x-ptTopLeft.x) + (ptBottomLeft.y-ptTopLeft.y)*(ptBottomLeft.y-ptTopLeft.y);

    if(temp>maxLength) maxLength = temp;

    maxLength = sqrt((double)maxLength);

    Point2f src[4], dst[4];
src[0] = ptTopLeft;            dst[0] = Point2f(0,0);
src[1] = ptTopRight;        dst[1] = Point2f(maxLength-1, 0);
src[2] = ptBottomRight;        dst[2] = Point2f(maxLength-1, maxLength-1);
src[3] = ptBottomLeft;        dst[3] = Point2f(0, maxLength-1);

Mat undistorted = Mat(Size(maxLength, maxLength), CV_8UC1);
cv::warpPerspective(sudoku, undistorted, cv::getPerspectiveTransform(src, dst), Size(maxLength, maxLength));
Mat undistortedThreshed = undistorted.clone();
adaptiveThreshold(undistorted, undistortedThreshed, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, 101, 1);
DigitRecognizer *dr = new DigitRecognizer();
bool b = dr->train("train-images.idx3-ubyte", "train-labels.idx1-ubyte");
int dist = ceil((double)maxLength/9);
Mat currentCell = Mat(dist, dist, CV_8UC1);

for(int j=0;j<9;j++)
{
    for(int i=0;i<9;i++)
    {
    	for(int y=0;y<dist && j*dist+y<undistortedThreshed.cols;y++)
        {

            uchar* ptr = currentCell.ptr(y);

            for(int x=0;x<dist && i*dist+x<undistortedThreshed.rows;x++)
            {
                ptr[x] = undistortedThreshed.at<uchar>(j*dist+y, i*dist+x);
            }
        }
        Moments m = cv::moments(currentCell, true);
        int area = m.m00;
        if(area > currentCell.rows*currentCell.cols/5)
        {
            int number = dr->classify(currentCell);
            printf("%d ", number);
        }
        else
        {
            printf("  ");
        }
        }
    printf(" ");
}

	imshow("thresholded",undistorted);
	waitKey(0);


	return 0;
}



DigitRecognizer::DigitRecognizer()
{
    knn = new KNearest();

}

DigitRecognizer::~DigitRecognizer()
{
    delete knn;
}
int DigitRecognizer::readFlippedInteger(FILE *fp)
{
    int ret = 0;

    BYTE *temp;

    temp = (BYTE*)(&ret);
    fread(&temp[3], sizeof(BYTE), 1, fp);
    fread(&temp[2], sizeof(BYTE), 1, fp);
    fread(&temp[1], sizeof(BYTE), 1, fp);

    fread(&temp[0], sizeof(BYTE), 1, fp);

    return ret;

}

bool DigitRecognizer::train(char *trainPath, char *labelsPath)
{
    FILE *fp = fopen(trainPath, "rb");

    FILE *fp2 = fopen(labelsPath, "rb");

    if(!fp || !fp2)

        return false;

    // Read bytes in flipped order
    int magicNumber = readFlippedInteger(fp);
    numImages = readFlippedInteger(fp);
    numRows = readFlippedInteger(fp);

    numCols = readFlippedInteger(fp);

    fseek(fp2, 0x08, SEEK_SET);


    if(numImages > MAX_NUM_IMAGES) numImages = MAX_NUM_IMAGES;

    //////////////////////////////////////////////////////////////////
    // Go through each training data entry and save a

    // label for each digit

    int size = numRows*numCols;

    CvMat *trainingVectors = cvCreateMat(numImages, size, CV_32FC1);

    CvMat *trainingClasses = cvCreateMat(numImages, 1, CV_32FC1);

    memset(trainingClasses->data.ptr, 0, sizeof(float)*numImages);

    BYTE *temp = new BYTE[size];
    BYTE tempClass=0;
    for(int i=0;i<numImages;i++)
    {

        fread((void*)temp, size, 1, fp);

        fread((void*)(&tempClass), sizeof(BYTE), 1, fp2);

        trainingClasses->data.fl[i] = tempClass;

        for(int k=0;k<size;k++)
            trainingVectors->data.fl[i*size+k] = temp[k]; ///sumofsquares;

    }

    knn->train(trainingVectors, trainingClasses);
    fclose(fp);

    fclose(fp2);

    return true;
}
int DigitRecognizer::classify(cv::Mat img)
{
    Mat cloneImg = preprocessImage(img);
    return knn->find_nearest(Mat_<float>(cloneImg), 1);
}

Mat DigitRecognizer::preprocessImage(Mat img)
{

    int rowTop=-1, rowBottom=-1, colLeft=-1, colRight=-1;

    Mat temp;
    int thresholdBottom = 50;
    int thresholdTop = 50;
    int thresholdLeft = 50;
    int thresholdRight = 50;
    int center = img.rows/2;
    for(int i=center;i<img.rows;i++)
    {
        if(rowBottom==-1)
        {
            temp = img.row(i);
            IplImage stub = temp;
            if(cvSum(&stub).val[0] < thresholdBottom || i==img.rows-1)
                rowBottom = i;

        }

        if(rowTop==-1)
        {
            temp = img.row(img.rows-i);
            IplImage stub = temp;
            if(cvSum(&stub).val[0] < thresholdTop || i==img.rows-1)
                rowTop = img.rows-i;

        }

        if(colRight==-1)
        {
            temp = img.col(i);
            IplImage stub = temp;
            if(cvSum(&stub).val[0] < thresholdRight|| i==img.cols-1)
                colRight = i;

        }

        if(colLeft==-1)
        {
            temp = img.col(img.cols-i);
            IplImage stub = temp;
            if(cvSum(&stub).val[0] < thresholdLeft|| i==img.cols-1)
                colLeft = img.cols-i;
        }
    }
    Mat newImg;

    newImg = newImg.zeros(img.rows, img.cols, CV_8UC1);

    int startAtX = (newImg.cols/2)-(colRight-colLeft)/2;

    int startAtY = (newImg.rows/2)-(rowBottom-rowTop)/2;

    for(int y=startAtY;y<(newImg.rows/2)+(rowBottom-rowTop)/2;y++)
    {
        uchar *ptr = newImg.ptr<uchar>(y);
        for(int x=startAtX;x<(newImg.cols/2)+(colRight-colLeft)/2;x++)
        {
            ptr[x] = img.at<uchar>(rowTop+(y-startAtY),colLeft+(x-startAtX));
        }
    }
    Mat cloneImg = Mat(numRows, numCols, CV_8UC1);

    resize(newImg, cloneImg, Size(numCols, numRows));

    // Now fill along the borders
    for(int i=0;i<cloneImg.rows;i++)
    {
        floodFill(cloneImg, cvPoint(0, i), cvScalar(0,0,0));

        floodFill(cloneImg, cvPoint(cloneImg.cols-1, i), cvScalar(0,0,0));

        floodFill(cloneImg, cvPoint(i, 0), cvScalar(0));
        floodFill(cloneImg, cvPoint(i, cloneImg.rows-1), cvScalar(0));
    }
    cloneImg = cloneImg.reshape(1, 1);

    return cloneImg;
}
