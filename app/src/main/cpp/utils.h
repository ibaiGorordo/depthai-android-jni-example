//
// Created by ibaig on 2/24/2022.
//

#ifndef DEPTHAI_ANDROID_JNI_EXAMPLE_UTILS_H
#define DEPTHAI_ANDROID_JNI_EXAMPLE_UTILS_H

#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#define LOG_TAG "depthaiAndroid"
#define log(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG, __VA_ARGS__)

extern "C" void readModelFromAsset(const char* model_path, std::vector<uint8_t>& model_buf, JNIEnv* env, jobject obj);
extern "C" cv::Mat imgframeToCvMat(const std::shared_ptr<dai::ImgFrame>& imgFrame);
extern "C" int colorDisparity(uint8_t disparity, float max_disparity);
extern "C" jintArray cvMatToBmpArray(JNIEnv* env, const cv::Mat& input_img);
extern "C" void draw_detections(cv::Mat frame, std::vector<dai::ImgDetection>& detections);

// MobilenetSSD label texts
//static const std::vector<std::string> labelMap = {"background", "aeroplane", "bicycle",     "bird",  "boat",        "bottle", "bus",
//                                                  "car",        "cat",       "chair",       "cow",   "diningtable", "dog",    "horse",
//                                                  "motorbike",  "person",    "pottedplant", "sheep", "sofa",        "train",  "tvmonitor"};

static const std::vector<std::string> labelMap = {
        "person",        "bicycle",      "car",           "motorbike",     "aeroplane",   "bus",         "train",       "truck",        "boat",
        "traffic light", "fire hydrant", "stop sign",     "parking meter", "bench",       "bird",        "cat",         "dog",          "horse",
        "sheep",         "cow",          "elephant",      "bear",          "zebra",       "giraffe",     "backpack",    "umbrella",     "handbag",
        "tie",           "suitcase",     "frisbee",       "skis",          "snowboard",   "sports ball", "kite",        "baseball bat", "baseball glove",
        "skateboard",    "surfboard",    "tennis racket", "bottle",        "wine glass",  "cup",         "fork",        "knife",        "spoon",
        "bowl",          "banana",       "apple",         "sandwich",      "orange",      "broccoli",    "carrot",      "hot dog",      "pizza",
        "donut",         "cake",         "chair",         "sofa",          "pottedplant", "bed",         "diningtable", "toilet",       "tvmonitor",
        "laptop",        "mouse",        "remote",        "keyboard",      "cell phone",  "microwave",   "oven",        "toaster",      "sink",
        "refrigerator",  "book",         "clock",         "vase",          "scissors",    "teddy bear",  "hair drier",  "toothbrush"};

#endif //DEPTHAI_ANDROID_JNI_EXAMPLE_UTILS_H
