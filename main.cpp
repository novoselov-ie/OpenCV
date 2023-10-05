#include <iostream>
#include <vector>
#include <filesystem>

#include <opencv2/opencv.hpp>

namespace fs = std::filesystem;

int ksize = 3;
int sigma = 1;
int thresh = 160;
double area;

std::vector<std::string> path_to_images;

void captureLoop();
std::pair<cv::Mat, cv::Mat> transImage(cv::Mat image);
void findImages(const std::string &pathToFile);

int main(int argc, char **argv)
{
    std::string pathToDirectory;
    std::cout << "Enter path to directory: " ;
    std::cin >> pathToDirectory;

    findImages(pathToDirectory);

    namedWindow("Contours", cv::WINDOW_AUTOSIZE);
    namedWindow("Blur", cv::WINDOW_AUTOSIZE);

    cv::createTrackbar("Sigma", "Contours", &sigma, 15, 0);
    cv::createTrackbar("Thresh", "Contours", &thresh, 255, 0);

    captureLoop();

    cv::destroyAllWindows();

    return 0;
}

void captureLoop(){

    cv::Mat image = cv::imread(path_to_images[0]);
    if (image.empty()){
        std::cout << "Cannot read the image: " << std::endl;
        return;
    }

    int index = 0;
    while(true){

        ksize = (sigma*5)|1;

        cv::imshow("Contours", transImage(image).first);
        cv::imshow("Blur", transImage(image).second);

        int key = cv::waitKey(30);
        if(key == 27)
            return;
        if(key == 13)
            std::cout << "Contour area: " << area << std::endl;
        if(key == 83 && index < path_to_images.size()-1){ // right
            index++;
            image = cv::imread(path_to_images[index]);
        }
        if(key == 81 && index > 0){ // left
            index--;
            image = cv::imread(path_to_images[index]);
        }
    }
}

std::pair<cv::Mat, cv::Mat> transImage(cv::Mat image){

    cv::Mat gray, blur, threshold;

    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    cv::medianBlur(gray, blur, ksize);
    cv::GaussianBlur(blur, blur, cv::Size(ksize, ksize), sigma, sigma);

    cv::threshold(blur, threshold, thresh, 255.0, cv::THRESH_BINARY);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(threshold, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat tmp (image.rows, image.cols, CV_8UC3, cv::Scalar::all(0));
    cv::drawContours(tmp, contours, -1, cv::Scalar(255, 255, 255), 1);

    for (int i = 0; i < contours.size(); i++) {
        area = cv::contourArea(contours[i]);
    }
    cv::putText(tmp, std::to_string(area), cv::Point(10, 20), cv::FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0,255,0), 1);

    return std::pair(tmp, blur);
}

void findImages(const std::string &pathToFile){
    for (const auto & entry : fs::directory_iterator(pathToFile)){
        if(entry.path().extension() == ".jpg" || entry.path().extension() == "png"){
            std::string S = entry.path().u8string();
            path_to_images.push_back(S);
        }
    }
}
