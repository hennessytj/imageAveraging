/***********************************************************************************
 * main.c written by Timothy Hennessy
 *
 * Description: Records performance for parallel image smoothing by
 * doubling the work and computing ratio.
 *
 * compile: see README.txt for details.
 * execute: ./a.out <num> path/<input_image_name>.jpg
 *********************************************************************************/
#include <ctime>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;
#define NUM_THREADS 10
Mat IMAGE, RESULT_IMAGE, ORIGINAL;


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
void neighborhoodAverage(int r, int c)
{
    CV_Assert(IMAGE.depth() == CV_8U);      // ensures pixel value range [0..255]
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
            if (i < 0 || i > IMAGE.rows - 1) break;     // avoid out of bounds rows
            if (j < 0 || j > IMAGE.cols - 1) continue;  // avoid out of bounds cols
            intensity = IMAGE.template at<Vec3b>(i, j); // store channels in 3 element vector BGR
            sumBlue  = sumBlue  + intensity.val[0];
            sumGreen = sumGreen + intensity.val[1];
            sumRed   = sumRed   + intensity.val[2];
            count++; // keeps count of total values used in sum
        }
    avgBlue  = saturate_cast<uchar>(sumBlue / count);
    avgGreen = saturate_cast<uchar>(sumGreen / count);
    avgRed   = saturate_cast<uchar>(sumRed / count);
    RESULT_IMAGE.template at<Vec3b>(r,c)[0] = avgBlue;
    RESULT_IMAGE.template at<Vec3b>(r,c)[1] = avgGreen;
    RESULT_IMAGE.template at<Vec3b>(r,c)[2] = avgRed;
}

/*********************************   partition  ********************************
 * void *partition(void *p)
 *
 * Description: Partitions image averaging by dividing rows amongst
 * threads.  Every thread calls neighborhoodAverage for each of its assigned
 * rows.
 *
 * Process:
 * 1.) Divide work.
 * 2.) Call neighborhoodAverage.
 * 3.) Threads exit.
 *
 * Parameter     Direction   Description
 * ----------------------------------------------------------------------------
 * p             in          thread tid used for division of work amongst
 *                           threads.
 *
 * Returns       Method      Description
 * ----------------------------------------------------------------------------
 * N/A
 *
 * NOTES:
 * - Since all arrays are global, the arrays are visible to all functions in
 *   this file.
 ******************************************************************************/
void *partition(void *p)
{
    int i, j;
    long tid = (long) p;
    int numRows = IMAGE.rows / NUM_THREADS;
    int remainingRows = IMAGE.rows % NUM_THREADS;
    int startRow =  numRows * (int) tid;
    int endRow;
    // last thread is one less than NUM_THREADS [0..NUM_THREADS)
    // last thread is assigned remaining number of rows, in the worst case is
    // N - 1
    if (tid == NUM_THREADS - 1)
    {
        endRow = numRows * (int) tid + numRows + remainingRows;
    }
    else
    {
        endRow = numRows * (int) tid + numRows;
    }
    
    // Conduct averaging operation
    for (i = startRow; i < endRow; i++)
        for (j = 0; j < IMAGE.cols; j++)
            neighborhoodAverage(i, j);
    
    pthread_exit(NULL);
}

/*******************************   runParallel   ********************************
 * void runParallel()
 *
 * Description: Initial thread management function.  Spins up threads.
 *
 * Process:
 * 1.) Create NUM_THREADS (constant in define.h) threads.
 * 2.) Send threads to partition function, aka entry point function.
 * 3.) Join all threads.
 *
 * Parameter     Direction   Description
 * ----------------------------------------------------------------------------
 * N/A
 *
 * Returns       Method      Description
 * ----------------------------------------------------------------------------
 * N/A
 *
 * NOTES:
 * - See pthreads documentation for more specific information.
 ******************************************************************************/
void runParallel()
{
    long t;
    void *status;
    pthread_t tid[NUM_THREADS];
    // Create threads and partition work
    for (t = 0; t < NUM_THREADS; t++)
        pthread_create(&tid[t], NULL, partition, (void *) t);
    // Join threads
    for (t = 0; t < NUM_THREADS; t++)
        pthread_join(tid[t], &status);
}

int main(int argc, char** argv)
{
    int n = atoi(argv[1]);
    char *imageName = argv[2];
    int t, averageOps = 1;
    
    // Read in image used in averaging operation
    IMAGE = imread(imageName, 1);
    
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
    else if (!imageName || !IMAGE.data)
    {
        cout << "No image data \n" << endl;
        cout << "usage: " << argv[0];
        cout << " number_of_avgs path_to_image path_to_output" << endl;
        return -1;
    }
    
    RESULT_IMAGE = IMAGE.clone();
    ORIGINAL = IMAGE.clone();
    
    // Conduct n averages of image and record time
    double prev_elapsed_secs = 0.0, curr_elapsed_secs = 0.0, total_time = 0.0;
    clock_t begin, end;
    while (averageOps <= n)
    {
        begin = clock();
        for (t = 0; t < averageOps; t++)
        {
            runParallel();
            IMAGE = RESULT_IMAGE.clone();
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
        IMAGE = ORIGINAL.clone(); // reset to original image
    }
    
    return 0;
}
