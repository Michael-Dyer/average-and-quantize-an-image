#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <time.h>
#include <vector>
#include <conio.h>
#include <dirent.h>
#include <windows.h>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/utils/filesystem.hpp>

#include <sys/stat.h>
#include <sys/types.h>


using namespace cv;
using namespace std;

Mat remove_lines(Mat og);
Mat add_lines(Mat og);
Mat intensity(Mat og, int intensity);
Mat up_average(Mat og);
Mat down_average(Mat og);

//use u_char and read as greyscale

int main(int argc, const char** argv) {
	
	uchar input = 'y';

	int sampling_method = 1;
	int depth = 1;
	int inten = 1;

	
	Mat image;

	try {


		//these are the keys for the comand line parser
		String keys =
			"{s sampling_method|1     | change the method of sampling }"
			"{d depth | 1     | change the depth}"
			"{i intesity| 1     | number of levels for downsampling }"
			"{h help  |      | show help message}"     // optional, show help optional
			"{@dir | <none>        | Input file}";
			

		
		CommandLineParser parser(argc, argv, keys);
		if (parser.has("h") || parser.has("help")) {
			cout << "To run program, execute; program_name " ;
			cout << " -h -s sampling_method -d depth -i intensity imagefile" << endl;
			cout << "Output file will be output_file " << endl;
			cout << "press q to exit image" << endl;
			parser.printMessage();
			return 0;
		}
		

		int sampling_method = parser.get<int>("s");
		int depth = parser.get<int>("d");
		int inten = parser.get<int>("i");

		if (inten < 1 || inten > 7) {
			cout << "Sorry, intesity must be between 1 and 7" << endl;
			return -1;
		}

		
		if (sampling_method  < 1 || sampling_method > 2) {
			cout << "Sorry, sampling_method must be 0 or 1" << endl;
			return -1;
		}


		//gets mandatory value, the original file from the parser
		String og_file = parser.get<String>(0);
		og_file =  og_file + ".png";
		cout << og_file << endl;

		
		
		Mat og_img = imread(og_file, IMREAD_COLOR);
		//Mat og_img = imread(og_file, IMREAD_GRAYSCALE);
		

		if (og_img.empty())
		{
			std::cout << "Could not read the image: " << og_file << std::endl;
			return 1;
		}



		//og_img = remove_lines(og_img);
		//og_img = add_lines(og_img);


		//og_img = down_average(og_img);
		//og_img = up_average(og_img);

		//og_img = intensity(og_img, inten);



		//this stuff is for display~~
		//namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
		//moveWindow("Display window", 0, 0);
		/*do {

			
			imshow("img", og_img);
				
			input = waitKey();
			


		} while (input != 'q');
		*/
		
		Mat down_mat = og_img;
		Mat up_mat;
		
		for (int i = 1; i <= depth; i++) {

			if (sampling_method == 1) {//delete method

				down_mat = remove_lines(down_mat);
				
				if (i == 1) {
					down_mat = intensity(down_mat, inten);
				}

				for (int y = 0; y < i; y++) {
					cout << "going up" << endl;
					//first instance
					if (y == 0) {
						up_mat = add_lines(down_mat);
					}
					//any other time
					else {
						up_mat = add_lines(up_mat);
					}
					
				}
				
				cout << "the current level is " << i << endl;

				imshow(to_string(i), up_mat);
				input = waitKey();

			}
			
			if (sampling_method == 2) {//average method
				

				down_mat = down_average(down_mat);
				
				if (i == 1) {
					
					down_mat = intensity(down_mat, inten);
				}

				for (int y = 0; y < i; y++) {
					
					//first instance
					if (y == 0) {
						up_mat = up_average(down_mat);
					}
					//any other time
					else {
						up_mat = up_average(up_mat);
					}
					
				}
				
				cout << "the current level is " << i << endl;

				imshow(to_string(i), up_mat);
				input = waitKey();

					


			}

		}//end of for loop


		cout << "end";


	}
	catch (std::string& str) {
		std::cerr << "Error: " << argv[0] << ": " << str << std::endl;
		return (1);
	}
	catch (cv::Exception& e) {
		std::cerr << "Error: " << argv[0] << ": " << e.msg << std::endl;
		return (1);
	}

	return (0);
}



Mat remove_lines(Mat og) {
	Mat new_mat = cv::Mat::zeros(og.rows/2, og.cols/2, og.type());
	
	//new_mat = og;
	for (int i = 0; i < og.rows; i++) {
		for (int j = 0; j < og.cols; j++) {
			if (j % 2 == 0 && i % 2 == 0) {	
				//if lines are odd remove

				//if greyscale
				if (og.channels() == 1) {
					new_mat.at<uchar>(i / 2, j / 2) = og.at<uchar>(i, j);
				}

				else {
					new_mat.at<Vec3b>(i / 2, j / 2) = og.at<Vec3b>(i, j);
				}
			}
		}

	}

	return new_mat;
}

Mat add_lines(Mat og) {
	//double the number of rows and cols
	Mat new_mat = cv::Mat::zeros(og.rows*2, og.cols*2, og.type());
	int x = 0;
	int y = 0;
	//new_mat = og;
	

	for (int i = 0; i < new_mat.rows; i++) {
		for (int j = 0; j < new_mat.cols; j++) {
			
			//double up every line

			if (og.channels() == 1) {
				new_mat.at<uchar>(i, j) = og.at<uchar>(i / 2, j / 2);
			
			}
			else {
				new_mat.at<Vec3b>(i, j) = og.at<Vec3b>(i / 2, j / 2);
			}
		}
		
	}

	return new_mat;
}

Mat up_average(Mat og) {
	Mat new_mat = cv::Mat::zeros(og.rows * 2, og.cols * 2, og.type());
	resize(og, new_mat, new_mat.size(), 0, 0, cv::INTER_LANCZOS4);
	
	return new_mat;
}

Mat down_average(Mat og) {
	Mat new_mat = cv::Mat::zeros(og.rows / 2, og.cols / 2, og.type());
	resize(og, new_mat, new_mat.size(), 0, 0, cv::INTER_LANCZOS4);

	return new_mat;
}

Mat intensity(Mat og, int intensity) {
	Mat new_mat = cv::Mat::zeros(og.rows, og.cols, og.type());


	for (int i = 0; i < new_mat.rows; i++) {
		for (int j = 0; j < new_mat.cols; j++) {

			if (og.channels() == 1) {
				new_mat.at<uchar>(i, j) = (og.at<uchar>(i, j) >> intensity) << intensity;
			}

			else {
				Vec3b pix = og.at<Vec3b>(i, j);
				pix[0] = (pix[0] >> intensity) << intensity;
				pix[1] = (pix[1] >> intensity) << intensity;
				pix[2] = (pix[2] >> intensity) << intensity;
				new_mat.at<Vec3b>(i, j) = pix;
			}


		}

	}

	return new_mat;
}

