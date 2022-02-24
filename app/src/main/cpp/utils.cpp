//
// Created by ibaig on 2/24/2022.
//

#include <opencv2/imgproc.hpp>
#include "depthai/depthai.hpp"

#include "utils.h"


extern "C" void readModelFromAsset(const char* model_path, std::vector<uint8_t>& model_buf, JNIEnv* env, jobject obj)
{
    jclass clazz = env->GetObjectClass(obj); // or env->FindClass("com/example/myapp/MainActivity");
    jmethodID method =
            env->GetMethodID(clazz, "getAssetManager", "()Landroid/content/res/AssetManager;");
    jobject jam = env->CallObjectMethod(obj, method);

    AAssetManager *assetManager = nullptr;
    if(jam)
    {
        assetManager = AAssetManager_fromJava(env, jam);
    }

    if (assetManager)
    {
        AAsset* assetDescriptor = AAssetManager_open(assetManager, model_path, AASSET_MODE_BUFFER);
        if (assetDescriptor != nullptr)
        {
            size_t fileLength = AAsset_getLength(assetDescriptor);

            model_buf.resize(fileLength);
            AAsset_read(assetDescriptor, model_buf.data(), model_buf.size());

            AAsset_close(assetDescriptor);
        }
    }
}

cv::Mat getFrame(const std::shared_ptr<dai::ImgFrame>& imgFrame) {
    // Convert to cv::Mat. If deepCopy enabled, then copy pixel data, otherwise reference only
    cv::Mat mat;
    cv::Size size = {0, 0};
    int type = 0;

    switch(imgFrame->getType()) {
        case dai::RawImgFrame::Type::RGB888i:
        case dai::RawImgFrame::Type::BGR888i:
        case dai::RawImgFrame::Type::BGR888p:
        case dai::RawImgFrame::Type::RGB888p:
            size = cv::Size(imgFrame->getWidth(), imgFrame->getHeight());
            type = CV_8UC3;
            break;

        case dai::RawImgFrame::Type::YUV420p:
        case dai::RawImgFrame::Type::NV12:
        case dai::RawImgFrame::Type::NV21:
            size = cv::Size(imgFrame->getWidth(), imgFrame->getHeight() * 3 / 2);
            type = CV_8UC1;
            break;

        case dai::RawImgFrame::Type::RAW8:
        case dai::RawImgFrame::Type::GRAY8:
            size = cv::Size(imgFrame->getWidth(), imgFrame->getHeight());
            type = CV_8UC1;
            break;

        case dai::RawImgFrame::Type::GRAYF16:
            size = cv::Size(imgFrame->getWidth(), imgFrame->getHeight());
            type = CV_16FC1;
            break;

        case dai::RawImgFrame::Type::RAW16:
            size = cv::Size(imgFrame->getWidth(), imgFrame->getHeight());
            type = CV_16UC1;
            break;

        case dai::RawImgFrame::Type::RGBF16F16F16i:
        case dai::RawImgFrame::Type::BGRF16F16F16i:
        case dai::RawImgFrame::Type::RGBF16F16F16p:
        case dai::RawImgFrame::Type::BGRF16F16F16p:
            size = cv::Size(imgFrame->getWidth(), imgFrame->getHeight());
            type = CV_16FC3;
            break;

        case dai::RawImgFrame::Type::BITSTREAM:
        default:
            size = cv::Size(static_cast<int>(imgFrame->getData().size()), 1);
            type = CV_8UC1;
            break;
    }

    // Check if enough data
    long requiredSize = CV_ELEM_SIZE(type) * size.area();
    long actualSize = static_cast<long>(imgFrame->getData().size());
    if(actualSize < requiredSize) {
        throw std::runtime_error("ImgFrame doesn't have enough data to encode specified frame, required " + std::to_string(requiredSize) + ", actual "
                                 + std::to_string(actualSize) + ". Maybe metadataOnly transfer was made?");
    } else if(actualSize > requiredSize) {
        // FIXME doesn't build on Windows (multiple definitions during link)
        // spdlog::warn("ImgFrame has excess data: actual {}, expected {}", actualSize, requiredSize);
    }
    if(imgFrame->getWidth() <= 0 || imgFrame->getHeight() <= 0) {
        throw std::runtime_error("ImgFrame metadata not valid (width or height = 0)");
    }

    mat = cv::Mat(size, type, imgFrame->getData().data());

    return mat;
}

// Ref: https://github.com/luxonis/depthai-core/blob/5cafb68da68e7242e94c7ebd038baa919f7e4835/src/opencv/ImgFrame.cpp#L95
extern "C" cv::Mat imgframeToCvMat(const std::shared_ptr<dai::ImgFrame>& imgFrame){

    cv::Mat frame = getFrame(imgFrame);
    cv::Mat output;

    switch(imgFrame->getType()) {
        case dai::RawImgFrame::Type::RGB888i:
            output = frame.clone();
            break;

        case dai::RawImgFrame::Type::BGR888i:
            cv::cvtColor(frame, output, cv::ColorConversionCodes::COLOR_BGR2RGB);
            break;

        case dai::RawImgFrame::Type::RGB888p: {
            cv::Size s(imgFrame->getWidth(), imgFrame->getHeight());
            std::vector<cv::Mat> channels;
            // RGB
            channels.push_back(cv::Mat(s, CV_8UC1, imgFrame->getData().data() + s.area() * 0));
            channels.push_back(cv::Mat(s, CV_8UC1, imgFrame->getData().data() + s.area() * 1));
            channels.push_back(cv::Mat(s, CV_8UC1, imgFrame->getData().data() + s.area() * 2));
            cv::merge(channels, output);
        } break;

        case dai::RawImgFrame::Type::BGR888p: {
            cv::Size s(imgFrame->getWidth(), imgFrame->getHeight());
            std::vector<cv::Mat> channels;
            // BGR
            channels.push_back(cv::Mat(s, CV_8UC1, imgFrame->getData().data() + s.area() * 2));
            channels.push_back(cv::Mat(s, CV_8UC1, imgFrame->getData().data() + s.area() * 1));
            channels.push_back(cv::Mat(s, CV_8UC1, imgFrame->getData().data() + s.area() * 0));
            cv::merge(channels, output);
        } break;

        case dai::RawImgFrame::Type::YUV420p:
            cv::cvtColor(frame, output, cv::ColorConversionCodes::COLOR_YUV2RGB_IYUV);
            break;

        case dai::RawImgFrame::Type::NV12:
            cv::cvtColor(frame, output, cv::ColorConversionCodes::COLOR_YUV2RGB_NV12);
            break;

        case dai::RawImgFrame::Type::NV21:
            cv::cvtColor(frame, output, cv::ColorConversionCodes::COLOR_YUV2RGB_NV21);
            break;

        case dai::RawImgFrame::Type::RAW8:
        case dai::RawImgFrame::Type::RAW16:
        case dai::RawImgFrame::Type::GRAY8:
        case dai::RawImgFrame::Type::GRAYF16:
            output = frame.clone();
            break;

        default:
            output = frame.clone();
            break;
    }

    return output;
}

extern "C" int colorDisparity(uint8_t disparity, float max_disparity)
{
    // Ref: https://www.particleincell.com/2014/colormap

    int r,g,b;

    auto a=(1.0f-(float)disparity/max_disparity)*5.0f;
    auto X=(int) floor(a);
    auto Y=(int) (255*(a-X));
    switch(X)
    {
        case 0: r=255;g=Y;b=0;break;
        case 1: r=255-Y;g=255;b=0;break;
        case 2: r=0;g=255;b=Y;break;
        case 3: r=0;g=255-Y;b=255;break;
        case 4: r=Y;g=0;b=255;break;
        case 5: r=255;g=0;b=255;break;
        default: r=0;g=0;b=0;break;
    }

    // Combine the different channels to get the integer value for the Bitmap pixel
    return 255 << 24 | (r << 16) | (g << 8) | b;
}

extern "C" void draw_detections(cv::Mat frame, std::vector<dai::ImgDetection>& detections) {
    auto color = cv::Scalar(255, 0, 0);
    // nn data, being the bounding box locations, are in <0..1> range - they need to be normalized with frame width/height
    for(auto& detection : detections) {
        int x1 = detection.xmin * frame.cols;
        int y1 = detection.ymin * frame.rows;
        int x2 = detection.xmax * frame.cols;
        int y2 = detection.ymax * frame.rows;

        uint32_t labelIndex = detection.label;
        std::string labelStr = std::to_string(labelIndex);
        if(labelIndex < labelMap.size()) {
            labelStr = labelMap[labelIndex];
        }

        log("Detection: %s [%f,%f,%f,%f], %f",labelStr.c_str(), detection.xmin, detection.ymin, detection.xmax, detection.ymax, detection.confidence);

        cv::putText(frame, labelStr, cv::Point(x1 + 10, y1 + 20), cv::FONT_HERSHEY_TRIPLEX, 0.5, color);
        std::stringstream confStr;
        confStr << std::fixed << std::setprecision(2) << detection.confidence * 100;
        cv::putText(frame, confStr.str(), cv::Point(x1 + 10, y1 + 40), cv::FONT_HERSHEY_TRIPLEX, 0.5, color);
        cv::rectangle(frame, cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2)), color, cv::FONT_HERSHEY_SIMPLEX);
    }

}

// Ref: https://stackoverflow.com/a/36792470/13706271
extern "C" jintArray cvMatToBmpArray(JNIEnv* env, const cv::Mat& input_rgb_img)
{
    auto imgData = input_rgb_img.data;

    uint image_size = input_rgb_img.cols*input_rgb_img.rows;

    jintArray result = env->NewIntArray(image_size);
    jint* result_e = env->GetIntArrayElements(result, 0);

    int j = 0;
    for (int i = 0; i < image_size*3; i+=3)
    {
        int red = imgData[i];
        int green = imgData[i + 1];
        int blue = imgData[i + 2];

        result_e[j] = 255 << 24 | (red << 16) | (green << 8) | blue;
        j++;
    }

    env->ReleaseIntArrayElements(result, result_e, 0);
    return result;
}

