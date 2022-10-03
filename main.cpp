#include <iostream>

#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/matx.hpp>

int main() {

  std::string path;
  int skip = 5;

  std::cout << "Welcome to img from video, made by puccj.\n\n";
  std::cout << "Insert file name (with extension): ";
  std::cin >> path;
  std::cout << "Insert out of many frame to skip for each one drawn: ";
  std::cin >> skip;
  
  cv::VideoCapture cap(path);

  int frameCount = 0;
  cv::Mat list[1];

  cv::namedWindow("Capture");
  cv::namedWindow("Detection");

  cv::Mat bg;
  cap.read(bg);
  cv::Mat frame, frame_HSV, frame_threshold;
  while (true) {
    for (int i = 0; i <= skip; i++)
      cap >> frame;

    if(frame.empty())
      break;

    cvtColor(frame, frame_HSV, cv::COLOR_BGR2HSV); // Convert from BGR to HSV colorspace
    // Detect the object based on HSV Range Values
    inRange(frame_HSV, cv::Scalar(0,75,40), cv::Scalar(180,255,255), frame_threshold);
    
    // Show the frames
    imshow("Capture", frame);
    imshow("Detection", frame_threshold);
    char key = (char) cv::waitKey(30);
    if (key == 'q' || key == 27)
      break;

    //write the arrow on top of bg image
    std::vector<cv::Point> whitePixels;
    cv::findNonZero(frame_threshold, whitePixels);

    for (int i = 0; i < whitePixels.size(); i++)
    bg.at<cv::Vec3b>(whitePixels[i]) = {0,0,255};

    cv::imshow("Result", bg);
  }
  
  cv::imwrite(path + '(' + std::to_string(skip) + ")-result.png", bg);
  std::cout << "The result has been saved. See you!\n";

  return 0;
}