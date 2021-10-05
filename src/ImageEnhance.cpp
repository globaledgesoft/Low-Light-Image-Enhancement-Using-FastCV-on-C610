// Image enhancement documentation 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fastcv.h"
#include <sys/time.h>
#include "opencv2/opencv.hpp"
#include <getopt.h>

using namespace cv;
using namespace std;

Mat guidedFilterMono(Mat src, float radius, float epsilon, bool mono )
{
/*
   guided filter code implementation is adapted from below paper
   
 referenv=ce Citations:
   * [Guided Image Filtering](http://kaiminghe.com/eccv10/), by Kaiming He, Jian Sun, and Xiaoou Tang, in TPAMI 2013
   * [Fast Guided Filter](https://arxiv.org/abs/1505.00996), by Kaiming He and Jian Sun, arXiv 2015
*/

  /*----------------------------------START OF FASTCV CODE------------------------------------*/
    fcvMemInit();
    uint8_t *img = (uint8_t *)fcvMemAlloc((src.cols* src.rows), 16);
    if (mono == true)
           fcvColorRGB888ToGrayu8((const uint8_t*)src.data, src.cols, src.rows, 0, img, 0);	
    else
        img  = src.data;
    float *P = (float *)fcvMemAlloc((src.cols* src.rows*4), 16);
    float *I = (float *)fcvMemAlloc((src.cols* src.rows*4), 16);
    for (int i = 0; i< src.cols* src.rows;i++){
        *(I + i) = (float) (*(img + i))/255.0;
        *(P + i) = (float) (*(img + i))/255.0;   // GUIDED IMAGE ALSO TAKEN SAME AS INPUT IMAGE
    }

    float *I2 = (float *)fcvMemAlloc((src.cols* src.rows*4), 16);
    float *P2 = (float *)fcvMemAlloc((src.cols* src.rows*4), 16);
    float *MeanI = (float *)fcvMemAlloc((src.cols* src.rows*4), 16);
    float *MeanP = (float *)fcvMemAlloc((src.cols* src.rows*4), 16);
    float *CorrI = (float *)fcvMemAlloc((src.cols* src.rows*4), 16);
    float *CorrP = (float *)fcvMemAlloc((src.cols* src.rows*4), 16);
    float *MeanI2 = (float *)fcvMemAlloc((src.cols* src.rows*4), 16);
    float *a = (float *)fcvMemAlloc((src.cols* src.rows*4), 16);
    float *b = (float *)fcvMemAlloc((src.cols* src.rows*4), 16);
    float *CovI = (float *)fcvMemAlloc((src.cols* src.rows*4), 16);
    float *VarI = (float *)fcvMemAlloc((src.cols* src.rows*4), 16);
    float *MeanA = (float *)fcvMemAlloc((src.cols* src.rows*4), 16);
    float *MeanB = (float *)fcvMemAlloc((src.cols* src.rows*4), 16);
    float *out = (float *)fcvMemAlloc((src.cols* src.rows*4), 16);
    uint8_t *res = (uint8_t *)fcvMemAlloc((src.cols* src.rows), 16);


    fcvElementMultiplyf32((const float*)I, src.cols, src.rows, src.cols*4,(const float*)I, src.cols*4, I2, src.cols*4);    
    fcvBoxFilterNxNf32((const float*)I, src.cols, src.rows, src.cols*4, 2*radius+1, MeanI, src.cols*4);
    fcvElementMultiplyf32((const float*)P, src.cols, src.rows, src.cols*4,(const float*)P, src.cols*4, P2, src.cols*4);    
    fcvBoxFilterNxNf32((const float*)P, src.cols, src.rows, src.cols*4, 2*radius+1, MeanP, src.cols*4);
    fcvBoxFilterNxNf32((const float*)I2, src.cols, src.rows, src.cols*4, 2*radius+1, CorrI, src.cols*4);
    fcvBoxFilterNxNf32((const float*)P2, src.cols, src.rows, src.cols*4, 2*radius+1, CorrP, src.cols*4);
    fcvElementMultiplyf32((const float*)MeanI, src.cols, src.rows, src.cols*4,(const float*)MeanI, src.cols*4, MeanI2, src.cols*4);  


    for (int i = 0; i< src.cols* src.rows;i++){
        *(VarI + i) =  (*(CorrI + i) - *(MeanI2 + i) );
        *(CovI + i) =  (*(CorrP + i) - (*(MeanI + i) * (*(MeanP + i) ))  );
        *(a + i) = *(CovI + i)/ (*(VarI + i) + epsilon);
        *(b + i) = *(MeanI + i) - (*(a + i) * (*(MeanI + i)));  
    }

    fcvBoxFilterNxNf32((const float*)a, src.cols, src.rows, src.cols*4, 2*radius+1, MeanA, src.cols*4);
    fcvBoxFilterNxNf32((const float*)b, src.cols, src.rows, src.cols*4, 2*radius+1, MeanB, src.cols*4);
    for (int i = 0; i< src.cols* src.rows;i++){
        *(out + i) = 255*((*(MeanA + i) * (*(I +i))) +  (*(MeanB + i)));
        *(res + i) = (uint8_t)(*(out + i));
    }

	Mat dst(src.size(), CV_8UC1);
        memcpy(dst.data, res, (src.cols)*(src.rows)*sizeof(uint8_t));


    // FREEING ALLOCATED MEMORY
    fcvMemFree(I); fcvMemFree(I2); fcvMemFree(P); fcvMemFree(P2); fcvMemFree(MeanP); fcvMemFree(MeanI); fcvMemFree(MeanI2); fcvMemFree(CovI);fcvMemFree(CorrI);fcvMemFree(CorrP);
	fcvMemFree(VarI); fcvMemFree(a); fcvMemFree(b); fcvMemFree(MeanA); fcvMemFree(MeanB); fcvMemFree(out); fcvMemFree(res);
    fcvMemDeInit();
    return dst;
}

Mat guidedFilterColor(Mat src, float radius, float epsilon)
{
    // guided filter for color image 
    Mat channel[3];
    Mat R, G, B, colorOutput;
    split(src, channel);
    
    R = guidedFilterMono(channel[2], radius, epsilon, false);
    G = guidedFilterMono(channel[1], radius, epsilon, false);
    B = guidedFilterMono(channel[0], radius, epsilon, false);

 vector<Mat> out;
    out.push_back(B);
    out.push_back(G);
    out.push_back(R);
    merge(out, colorOutput);
    imwrite("colorout.jpg",colorOutput);
    return colorOutput;
}

Mat guidedFilter(Mat src, float radius, float epsilon, bool mono)
{

    Mat dst; 
    int numChannel = src.channels();
    if(numChannel == 1)
       dst = guidedFilterMono(src, radius, epsilon, false);
    else if((numChannel == 3) && mono== true)
       
       dst = guidedFilterMono(src, radius, epsilon, true);
    else if (numChannel == 3)    
       dst = guidedFilterColor(src, radius, epsilon);
    return dst;
}


Mat bilateralfilterMono(Mat src)
{
    // Bilateral filter
    fcvMemInit();
    uint8_t *grey = (uint8_t *)fcvMemAlloc((src.cols* src.rows), 16);
	uint8_t *out = (uint8_t *)fcvMemAlloc((src.cols*src.rows), 16);
    if(src.channels() == 3)
       fcvColorRGB888ToGrayu8((const uint8_t*)src.data, src.cols, src.rows, 0, grey, 0);	
    else
       grey  = src.data;
    fcvBilateralFilterRecursiveu8((const uint8_t*)grey, src.cols,src.rows,src.cols,out,src.cols,0.03,0.1);  
	Mat dst(src.size(), CV_8UC1);
    memcpy(dst.data, out, (src.cols)*(src.rows)*sizeof(uint8_t));
    fcvMemFree(grey); fcvMemFree(out);
	fcvMemDeInit();
    return dst;
}


Mat medianfilterMono(Mat src)
{
    // median filter 
    fcvMemInit();
    uint8_t *grey = (uint8_t *)fcvMemAlloc((src.cols* src.rows), 16);
	uint8_t *out = (uint8_t *)fcvMemAlloc((src.cols*src.rows), 16);
    if(src.channels() == 3)
       fcvColorRGB888ToGrayu8((const uint8_t*)src.data, src.cols, src.rows, 0, grey, 0);	
    else
       grey  = src.data;
    fcvFilterMedian3x3u8_v3((const uint8_t*)grey, src.cols, src.rows,src.cols,out,0,FASTCV_BORDER_REPLICATE,0);
	Mat dst(src.size(), CV_8UC1);
    memcpy(dst.data, out, (src.cols)*(src.rows)*sizeof(uint8_t));
    fcvMemFree(grey); fcvMemFree(out);
	fcvMemDeInit();
    return dst;
}


int main(int argc, char *argv[])
{

   cv::VideoCapture cap;          

   Mat src,dst;
   int count = 1;
 
 
   if(!(strcmp(argv[2],"capture")))
   {
   		cap.open("qtiqmmfsrc ldc=TRUE !video/x-raw, format=NV12, width=1280, height=720, framerate=30/1 ! videoconvert ! appsink", CAP_GSTREAMER);
        count =10;       // to skip bluish image;
   }
   else 
      count  =1;

  while(count--) 
  {


   if( !(strcmp(argv[2],"capture")) )
   {
       if (cap.read(src) == false) 
       {
                std::cout << "Video camera is disconnected" << std::endl;
                std::cin.get();
                break;
       }

      if(count == 4)
    {

		if (!(strcmp(argv[1],"guided"))){
			float radius = 7.5; 
			float epsilon = 0.001;
		    dst = guidedFilter(src, radius, epsilon,false);         // for gray image output make an third argumment as true
		 }
		  else if(!(strcmp(argv[1],"bilateral")))
		    dst = bilateralfilterMono(src);
		  else if(!(strcmp(argv[1],"median")))
		    dst = medianfilterMono(src);

		         cv::imwrite("output.jpg", dst);
    }
      
   }else
   {
    	src = imread(argv[2],IMREAD_COLOR);
		if (!(strcmp(argv[1],"guided"))){
			float radius = 7.5; 
			float epsilon = 0.001;
		    dst = guidedFilter(src, radius, epsilon, false);
		}
		else if(!(strcmp(argv[1],"bilateral")))
		    dst = bilateralfilterMono(src);
		else if(!(strcmp(argv[1],"median")))
		    dst = medianfilterMono(src);

		cv::imwrite("output.jpg", dst);
	   }  
    }   
    cap.release();
    return 0;
}
