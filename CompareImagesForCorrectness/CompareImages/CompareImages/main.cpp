#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

#define FALSE    0
#define TRUE     1

int main(int argc, char *argv[])
{
    char *seqImg = argv[1];
    char *parImg = argv[2];
    int i,j, rows, cols;
    int bIdentical;
    int count = 0;
    Vec3b seqIntensity;
    Vec3b parIntensity;
    Mat sequential = imread(seqImg, 1);
    Mat parallel   = imread(parImg, 1);
    
    // Ensure command line arguments were read successfully
    if (argc != 3)
    {
        cout << "usage: " << argv[0];
        cout << " path_to_sequential_image path_to_parallel_image" << endl;
        return -1;
    }
    else if (!seqImg || !sequential.data || !parImg || !parallel.data)
    {
        cout << "No image data \n" << endl;
        cout << "usage: " << argv[0];
        cout << " path_to_sequential_image path_to_parallel_image" << endl;
        return -1;
    }
    
    // Must be same size to be identical
    if (sequential.rows != parallel.rows || sequential.cols != parallel.cols)
    {
        cout << "Error: Images are not of same dimensions." << endl;
        return -1;
    }
    
    // Ensure images are 3 channel BGR aka jpg format
    if (sequential.depth() != CV_8U || parallel.depth() != CV_8U)
    {
        cout << "Error: improper image type." << endl;
        return -1;
    }
    
    rows = sequential.rows;
    cols = parallel.cols;

    // Compare each images pixel to determine if identical
    bIdentical = TRUE;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            seqIntensity = sequential.at<Vec3b>(i, j);
            parIntensity = parallel.at<Vec3b>(i, j);
            if (seqIntensity != parIntensity) { bIdentical = FALSE; break; }
            /*
            if (seqIntensity.val[0] != parIntensity.val[0])
            {
                cout << seqIntensity.val[0] << " != " << parIntensity.val[0] << endl;
                count++;
            }
            if (seqIntensity.val[1] != parIntensity.val[1])
            {
                cout << seqIntensity.val[1] << " != " << parIntensity.val[1] << endl;
                count++;
            }
            if (seqIntensity.val[2] != parIntensity.val[2])
            {
                cout << seqIntensity.val[2] << " != " << parIntensity.val[2] << endl;
                count++;
            }
             */
        }
    }
    
    // Determine result
    if (bIdentical)
    {
        cout << "Images are identical!" << endl;
    }
    else
    {
        cout << "Images are not identical:(" << endl;
    }

    return 0;
}
