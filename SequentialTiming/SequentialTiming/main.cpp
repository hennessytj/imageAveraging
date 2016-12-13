/*********************************************************************************
 * main.c written by Timothy Hennessy
 *
 * Description: Performs a doubling time operation and prints out the ratio.
 *
 * compile: see README.txt for details.
 * execute: ./a.out <num> path/<input_image_name>.jpg
 ********************************************************************************/
#include <ctime>
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

/*********************** neighborhoodAverage ******************************
 * void neighborhoodAverage(const Mat& img, Mat& result, int r, int c)
 *
 * Description: Computes average of a neighborhood centered at (r,c) and
 * stores average into result.
 *
 * Process:
 * 1.) Set up variables for readability, computation, and indexing.
 * 2.) Assigns starting and ending values for indexes of neighborhood.
 * 3.) Iterate through neighborhood summing together every valid cell.
 * 4.) Compute each average and store in result.
 *
 * Parameter     Direction   Description
 * ------------------------------------------------------------------------
 * img           in          OpenCV Mat data structure, contains pointer to
 *                           2D array containing image pixels.
 * result        out         OpenCV Mat data structure used to store result
 *                           of neighborhood average.
 * r             in          Row value for cell at center of neighborhood.
 * c             in          Col value for cell at center of neighborhood.
 *
 * NOTES:
 * - Assumes the values for r and c are viable.  This function will not go
 *   out of bounds, but it will still try to compute the neighborhood
 *   centered at (r,c)
 **************************************************************************/
void neighborhoodAverage(const Mat& img, Mat& result, int r, int c)
{
    CV_Assert(img.depth() == CV_8U);        // ensures pixel value range [0..255]
    int startRow, endRow, startCol, endCol; // readability vars
    int sumBlue = 0, sumGreen = 0,          // averaging computation
    sumRed = 0, avgBlue, avgGreen, avgRed;
    int count = 0;
    Vec3b intensity;                        // stores 3-channel pixel data
    int i, j;                               // loop indices
    startRow = r - 1;
    endRow   = r + 1;
    startCol = c - 1;
    endCol   = c + 1;
    for (i = startRow; i <= endRow; i++)
        for (j = startCol; j <= endCol; j++)
        {
            if (i < 0 || i > img.rows - 1) break;     // avoid out of bounds rows
            if (j < 0 || j > img.cols - 1) continue;  // avoid out of bounds cols
            intensity = img.template at<Vec3b>(i, j); // store channels in 3 element vector BGR
            sumBlue  = sumBlue  + intensity.val[0];
            sumGreen = sumGreen + intensity.val[1];
            sumRed   = sumRed   + intensity.val[2];
            count++; // keeps count of total values used in sum
        }
    avgBlue  = saturate_cast<uchar>(sumBlue / count);
    avgGreen = saturate_cast<uchar>(sumGreen / count);
    avgRed   = saturate_cast<uchar>(sumRed / count);
    result.template at<Vec3b>(r,c)[0] = avgBlue;
    result.template at<Vec3b>(r,c)[1] = avgGreen;
    result.template at<Vec3b>(r,c)[2] = avgRed;
}

int main(int argc, char** argv)
{
    int i, j, t, averageOps = 1;
    int n = atoi(argv[1]);     // doubling limit
    char *imageName = argv[2]; // input image
    Mat image, averagedImage, original;
    
    // Read in image used in averaging operation
    image = imread(imageName, 1);
    
    // Ensure command line arguments were read successfully
    if (argc != 3)
    {
        cout << "usage: " << argv[0];
        cout << " number_of_avgs path_to_image path_to_output" << endl;
        return -1;
    }
    else if (n < 1)
    {
        cout << "Number of averaging iterations must be > 0\n " << endl;
        return -1;
    }
    else if (!imageName || !image.data)
    {
        cout << "No image data \n" << endl;
        cout << "usage: " << argv[0];
        cout << " number_of_avgs path_to_image path_to_output" << endl;
        return -1;
    }
    
    averagedImage = image.clone();
    original = image.clone();

    cout << "#    ratio   total" << endl;
    clock_t begin, end;
    double prev_elapsed_secs = 0.0, curr_elapsed_secs = 0.0, total_time = 0.0;
    while (averageOps <= n)
    {
        begin = clock();
        for (t = 0; t < averageOps; t++)
        {
            for (i = 0; i < image.rows; i++)
                for (j = 0; j < image.cols; j++)
                    neighborhoodAverage(image, averagedImage, i, j);
            image = averagedImage.clone();
        }
        end = clock();
        curr_elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        if (averageOps > 0)
        {
            cout << averageOps << " : " << curr_elapsed_secs / prev_elapsed_secs;
            cout << " : " << total_time << endl;
        }
        total_time += curr_elapsed_secs;
        prev_elapsed_secs = curr_elapsed_secs;
        averageOps *= 2; // double the amount of work
        image = original.clone(); // reset to original image
    }
    
    return 0;
}
