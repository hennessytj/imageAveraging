Written by: Timothy Hennessy

Summary:  Integrated OpenCV and neighborhood project to implement image smoothing.

Files: main.cpp, all other files are images used in testing

Compilation: It is important to review the below program requirements before compiling.  I used XCode and set it up with the below information.  I am not sure exactly how to compile it otherwise.  I do believe the details below provide the essential information required to get it working on your platform.

Program requirements:
- OpenCV 3.0-Dev must be installed.  There are no guarantees that this program will compile with other versions of OpenCV.
- Compiler must know where OpenCV header and lib files reside.
- Compiler must be provided OpenCV linker flags.
- There are two required command line arguments
o  First - absolute path to the image.  Image must be a properly formatted jpg file.
o Second Ð absolute path to where smoothed image will be saved.  Image requires thoughtful name and must be .jpg.  

Compiler header and lib files: 
1) Add Header to Search Paths: /usr/local/Cellar/opencv3/HEAD-8213e57_4/include	
2) Add Library to Search Paths: /usr/local/Cellar/opencv3/HEAD-8213e57_4/lib
     NOTE: These are examples from my set up.

Compiler linker flags: -lpthread -lopencv_calib3d -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videoio -lopencv_videostab
NOTE: Most of these are not used, but I truthfully do not know which ones are necessary.

Execution: ./a.out num input_image_path/image.jpg output_image_path/image.jpg

Interpretation: This program implements a rather small neighborhood/filter/kernel for the image averaging/smoothing operation.  In some cases the output results are subtle.  Regardless, there should be a noticeable blurring effect when comparing the input image to the output image.  These results are rather basic and it might be worthwhile to run the averaging process multiple times from start to finish before generating the final output.  Another idea is to increase the neighborhood/filter/kernel size.  These considerations might be included in the next version.



