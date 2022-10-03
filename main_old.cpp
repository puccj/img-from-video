#include <iostream>
#include <random>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

int main() {
  const int BG_FRAMES = 50;

  std::string path = "ramsey-d0.2.mp4";
  cv::VideoCapture cap(path);

  double fps = cap.get(cv::CAP_PROP_FPS);

  //Get background
  std::cout << "Getting Background... ";
  
  cv::Mat frames[BG_FRAMES];  //stored frames to calculate the bkgnd with  

  std::random_device rd;
  std::uniform_int_distribution<int> dist(0, cap.get(cv::CAP_PROP_FRAME_COUNT)-1);
  for (int i = 0; i < BG_FRAMES; i++) {
    int rand = dist(rd);
    cap.set(cv::CAP_PROP_POS_FRAMES, rand);                 //set the frame id to read that particular frame
    cap.read(frames[i]);                                    //read that frame
    cv::cvtColor(frames[i], frames[i], cv::COLOR_BGR2GRAY); //convert it in B&W
  }

  int rows = frames[0].rows;
  int cols = frames[0].cols;
  
  cv::Mat background(rows, cols, CV_8UC1);  //store the background image

  for (int x = 0; x < cols; x++) {
    for (int y = 0; y < rows; y++) {
      //create an array with the (x,y) pixel of all frames
      uchar currentPixel[BG_FRAMES];

      for (int i = 0; i < BG_FRAMES; i++) {
        //insert sort: pos is the position where the element will be inserted
        int pos = i-1;
        while(pos>=0 && frames[i].at<uchar>(y,x) < currentPixel[pos]) {
          currentPixel[pos+1] = currentPixel[pos];
          pos--;
        }
        currentPixel[pos+1] = frames[i].at<uchar>(y,x);
      }
      //now currentPixel is a sorted array with (x,y) pixel by all frames.
      //gets the median value and write it to the (x,y) pixel in background image
      background.at<uchar>(y,x) = currentPixel[BG_FRAMES/2];
    }
  }
  
  cap.open(path);
  std::cout << "     Done\n";
  cv::imshow("BG", background);
  cv::waitKey(0);


  cv::Mat result = background.clone();
  
  int frameCount = 0;
  cv::Mat list[1];

  while (cap.isOpened()) {
    //read a frame
    std::cout << "0\n";
    cv::Mat thisFrame;
    cap.read(thisFrame);

    if (thisFrame.empty())
      break;

    //save the original frame
    cv::Mat origFrame;
    thisFrame.copyTo(origFrame);

    std::cout << "1\n";
    cv::cvtColor(thisFrame, thisFrame, cv::COLOR_BGR2GRAY);           //convert to grayscale
    std::cout << "2\n";
    cv::absdiff(thisFrame, background, thisFrame);                    //subtract background from image
    std::cout << "3\n";
    cv::threshold(thisFrame, thisFrame, 50, 255, cv::THRESH_BINARY);  //thresholding to convert to binary
    std::cout << "4\n";
    cv::dilate(thisFrame,thisFrame, cv::Mat(), cv::Point(-1,-1), 2);  //dilate the image (no inside dark regions)

    frameCount++;

    //skip if list is not already full
    if (frameCount < 1)
      continue;
    
    std::cout << "5\n";
    //find the contours and draw them in the bg
    std::vector<cv::Mat> contours;
    std::cout << "6\n";
    cv::findContours(thisFrame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::cout << "7\n";
    cv::drawContours(result, contours, -1, {255,0,0}, 1);

    std::cout << "8\n";
    cv::imshow("Result", result);
    cv::waitKey(1000/fps);
  }

  cv::imwrite("Result.png", result);

  return 0;
}