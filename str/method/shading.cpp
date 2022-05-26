#include "shading.hpp"


DITCameraTool::Algorithm::Shading::Shading(){
    cv::Mat image;
    std::string imagePath;
    DITCameraTool::Config DITConfig;
}


DITCameraTool::Algorithm::Shading::Shading(DITCameraTool::Config config, std::string filePath){
    imagePath = filePath;
    image = loadImage();
    algorithmConf = config.getAlgorithmConf();
    globalConf = config.getGlobalConf();
}

cv::Mat DITCameraTool::Algorithm::Shading::loadImage() const {
    cv::Mat figure = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);
    cv::Size figureSize(figure.size());
    if (figure.empty()){
        throw std::invalid_argument("Invalid image path. ("+imagePath+")");
    }
    return figure;
}

bool DITCameraTool::Algorithm::Shading::execute() const {
    bool resultBool = false;
    float BLOCKRATIO = std::stof((std::string)algorithmConf["BlockRatio"]); //BlockRatio = 0.1;
    cv::Size imageSize(image.size());
    int imageH = imageSize.height;
    int imageW = imageSize.width;
    int blockH = static_cast<float>(imageH)*BLOCKRATIO;
    int blockW = static_cast<float>(imageW)*BLOCKRATIO;
    int avgAreaLT = _fetchAvgPixel(0, 0, blockW, blockH);
    int avgAreaLB = _fetchAvgPixel(0, imageH-blockH, blockW, blockH);
    int avgAreaRT = _fetchAvgPixel(imageW-blockW, 0, blockW, blockH);
    int avgAreaRB = _fetchAvgPixel(imageW-blockW, imageH-blockH, blockW, blockH);
    int avgAreaCentre = _fetchAvgPixel(imageW/4, imageH/4, imageW/2, imageH/2);
    std::vector<int> avgAreaCorner{avgAreaLB, avgAreaLT, avgAreaRB, avgAreaRT};
    bool resultCentre = _detectCentre(avgAreaCentre);
    bool resultCornerShading = _detectCornerShading(avgAreaCentre, avgAreaCorner);
    bool resultCornerDiff = _detectCornerDiff(avgAreaCorner);
    printf("resultCentre: %d, resultCornerShading: %d, resultCornerDiff: %d\n", resultCentre, resultCornerShading, resultCornerDiff);
    if(resultCentre && resultCornerDiff && resultCornerShading){
        resultBool = true;
    }
    std::cout << (resultBool?"PASS":"NOT PASS") << std::endl;
    return resultBool;
}

int DITCameraTool::Algorithm::Shading::_fetchAvgPixel(int begin_x, int begin_y, int rectWidth, int rectHeight) const{
    cv::Rect imageRect(begin_x, begin_y, rectWidth, rectHeight);
    cv::Mat imageMat = image(imageRect);
    unsigned long long int accumulatePixel=0;
    int numPixel = rectWidth*rectHeight;
    for(int i=0; i<rectHeight; i++){
        for (int j=0; j<rectWidth; j++){
            accumulatePixel += imageMat.at<uchar>(i, j);
        }
    }
    int avgPixel =  accumulatePixel/numPixel;
    return avgPixel;
};

bool DITCameraTool::Algorithm::Shading::_detectCentre (int avgAreaCentre) const{
    int CENTER_UP = std::stoi((std::string)algorithmConf["Center_Up"]);
    int CENTER_LOW = std::stoi((std::string)algorithmConf["Center_Low"]);
    if(avgAreaCentre<CENTER_UP && avgAreaCentre>CENTER_LOW){
        return true;
    } else {
        return false;
    }
}

bool DITCameraTool::Algorithm::Shading::_detectCornerShading(int avgAreaCentre, std::vector<int> avgAreaVec) const{
    int PASSLEVEL = std::stoi((std::string)algorithmConf["PassLevel"]);
    int PASSLEVEL_UP = std::stoi((std::string)algorithmConf["PassLevel_Up"]);
    bool detectResult = true;
    for (int avgArea:avgAreaVec){
        double avgAreaShading = 100*(static_cast<double>(avgArea)/static_cast<double>(avgAreaCentre));
        detectResult = ( detectResult&& (avgAreaShading>PASSLEVEL )&& ( avgAreaShading<PASSLEVEL_UP ) );
    }
    return detectResult;
}

bool DITCameraTool::Algorithm::Shading::_detectCornerDiff(std::vector<int> avgAreaVec) const {
    int DIFF = std::stoi((std::string)algorithmConf["Diff"]);
    int maxVal = 0;
    int maxLoc = 0;
    int minVal = 256;
    int minLoc = 0;
    bool detectResult = false;
    for(int i = 0; i<avgAreaVec.size(); i++){
        if (avgAreaVec[i]>maxVal){
            maxVal = avgAreaVec[i];
            maxLoc = i;
        }
        if (avgAreaVec[i]<minVal){
            minVal = avgAreaVec[i];
            minLoc = i;
        }
    }
    if ((maxVal-minVal)< DIFF){
        detectResult = true;
    }
    return detectResult;
}
