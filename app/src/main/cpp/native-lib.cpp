#include <chrono>
#include <string>
#include <jni.h>

#include <libusb/libusb.h>
#include "opencv2/core.hpp"
#include "depthai/depthai.hpp"

#include "utils.h"

using namespace std;

std::shared_ptr<dai::Device> device;
shared_ptr<dai::DataOutputQueue> qRgb, qDepth, qDet;
cv::Mat detection_img;

// Neural network
std::vector<uint8_t> model_buffer;
static std::atomic<bool> syncNN{true};
std::vector<dai::ImgDetection> detections;

// Closer-in minimum depth, disparity range is doubled (from 95 to 190):
static std::atomic<bool> extended_disparity{true};
auto maxDisparity = extended_disparity ? 190.0f :95.0f;

// Better accuracy for longer distance, fractional disparity 32-levels:
static std::atomic<bool> subpixel{false};
// Better handling for occlusions:
static std::atomic<bool> lr_check{false};

extern "C"
JNIEXPORT void JNICALL
Java_com_example_depthai_1android_1jni_1example_MainActivity_startDevice(JNIEnv *env, jobject thiz, jstring model_path,
                        int rgbWidth, int rgbHeight) {

    // libusb
    auto r = libusb_set_option(nullptr, LIBUSB_OPTION_ANDROID_JNIENV, env);
    log("libusb_set_option ANDROID_JAVAVM: %s", libusb_strerror(r));

    // Connect to device and start pipeline
    device = make_shared<dai::Device>(dai::OpenVINO::VERSION_2021_4, dai::UsbSpeed::HIGH);

    bool oakD = device->getConnectedCameras().size() == 3;

    // Create pipeline
    dai::Pipeline pipeline;

    // Define source and output
    auto camRgb = pipeline.create<dai::node::ColorCamera>();
    auto xoutRgb = pipeline.create<dai::node::XLinkOut>();
    xoutRgb->setStreamName("rgb");

    // Properties
    camRgb->setPreviewSize(rgbWidth, rgbHeight);
    camRgb->setResolution(dai::ColorCameraProperties::SensorResolution::THE_1080_P);
    camRgb->setInterleaved(false);
    camRgb->setColorOrder(dai::ColorCameraProperties::ColorOrder::BGR);

    // NN
    auto detectionNetwork = pipeline.create<dai::node::YoloDetectionNetwork>();
//    auto detectionNetwork = pipeline.create<dai::node::MobileNetDetectionNetwork>();
    auto nnOut = pipeline.create<dai::node::XLinkOut>();
    nnOut->setStreamName("detections");

    // Load model blob
    std::vector<uint8_t> model_buf;
    const char * path = env->GetStringUTFChars(model_path, 0);
    readModelFromAsset(path, model_buffer, env, thiz);
    env->ReleaseStringUTFChars(model_path, path);
    auto model_blob = dai::OpenVINO::Blob(model_buffer);

    // Network specific settings
    detectionNetwork->setConfidenceThreshold(0.5f);
    detectionNetwork->setNumClasses(80);
    detectionNetwork->setCoordinateSize(4);
// Yolov3/v4 tiny
//    detectionNetwork->setAnchors({10, 14, 23, 27, 37, 58, 81, 82, 135, 169, 344, 319});
//    detectionNetwork->setAnchorMasks({{"side26", {1, 2, 3}}, {"side13", {3, 4, 5}}});
// Yolov5s
    detectionNetwork->setAnchors({10,13, 16,30, 33,23, 30,61, 62,45, 59,119, 116,90, 156,198, 373,326});
    detectionNetwork->setAnchorMasks({{"side52", {0, 1, 2}}, {"side26", {3, 4, 5}}, {"side13", {6, 7, 8}}});
    detectionNetwork->setIouThreshold(0.5f);
    detectionNetwork->setBlob(model_blob);
    detectionNetwork->setNumInferenceThreads(2);
    detectionNetwork->input.setBlocking(false);

    // Linking
    camRgb->preview.link(detectionNetwork->input);
    if(syncNN) {
        detectionNetwork->passthrough.link(xoutRgb->input);
    } else {
        camRgb->preview.link(xoutRgb->input);
    }

    detectionNetwork->out.link(nnOut->input);

    if(oakD){
        auto monoLeft = pipeline.create<dai::node::MonoCamera>();
        auto monoRight = pipeline.create<dai::node::MonoCamera>();
        auto stereo = pipeline.create<dai::node::StereoDepth>();
        auto xoutDepth = pipeline.create<dai::node::XLinkOut>();
        xoutDepth->setStreamName("depth");
        // Properties
        monoLeft->setResolution(dai::MonoCameraProperties::SensorResolution::THE_400_P);
        monoLeft->setBoardSocket(dai::CameraBoardSocket::LEFT);
        monoRight->setResolution(dai::MonoCameraProperties::SensorResolution::THE_400_P);
        monoRight->setBoardSocket(dai::CameraBoardSocket::RIGHT);

        stereo->initialConfig.setConfidenceThreshold(245);
        // Options: MEDIAN_OFF, KERNEL_3x3, KERNEL_5x5, KERNEL_7x7 (default)
        stereo->initialConfig.setMedianFilter(dai::MedianFilter::KERNEL_7x7);
        stereo->setLeftRightCheck(lr_check);
        stereo->setExtendedDisparity(extended_disparity);
        stereo->setSubpixel(subpixel);

        // Linking
        monoLeft->out.link(stereo->left);
        monoRight->out.link(stereo->right);
        stereo->disparity.link(xoutDepth->input);
    }

    device->startPipeline(pipeline);

    // Output queue will be used to get the rgb frames from the output defined above
    qRgb = device->getOutputQueue("rgb", 1, false);
    detection_img = cv::Mat(rgbHeight, rgbWidth, CV_8UC3);


    // Output queue will be used to get the nn output from the neural network node defined above
    qDet = device->getOutputQueue("detections", 1, false);

    if(oakD) {
        // Output queue will be used to get the rgb frames from the output defined above
        qDepth = device->getOutputQueue("depth", 1, false);
    }

}

extern "C" JNIEXPORT jintArray JNICALL
Java_com_example_depthai_1android_1jni_1example_MainActivity_imageFromJNI(
        JNIEnv* env,
        jobject /* this */) {

    std::shared_ptr<dai::ImgFrame> inRgb;
    if(syncNN) {
        inRgb = qRgb->get<dai::ImgFrame>();
    } else {
        inRgb = qRgb->tryGet<dai::ImgFrame>();
    }

    if(!inRgb) return nullptr;

    // Copy image data to cv img
    detection_img = imgframeToCvMat(inRgb);

    // Copy image data to Bitmap int array
    return cvMatToBmpArray(env, detection_img);
}

extern "C" JNIEXPORT jintArray JNICALL
Java_com_example_depthai_1android_1jni_1example_MainActivity_depthFromJNI(
        JNIEnv* env,
        jobject /* this */) {

    bool oakD = device->getConnectedCameras().size() == 3;
    if(!oakD){
        return env->NewIntArray(0);
    }

    auto inDepth =  qDepth->get<dai::ImgFrame>();;
    auto imgData = inDepth->getData();

    u_long image_size = imgData.size();
    jintArray result = env->NewIntArray(image_size);
    jint* result_e = env->GetIntArrayElements(result, NULL);

    for (int i = 0; i < image_size; i++)
    {
        // Convert the disparity to color
        result_e[i] = colorDisparity(imgData[i], maxDisparity);
    }

    env->ReleaseIntArrayElements(result, result_e, NULL);
    return result;
}


extern "C"
JNIEXPORT jintArray JNICALL
Java_com_example_depthai_1android_1jni_1example_MainActivity_detectionImageFromJNI(JNIEnv *env,
                                                                               jobject thiz) {
    std::shared_ptr<dai::ImgDetections> inDet;
    if(syncNN) {
        inDet = qDet->get<dai::ImgDetections>();
    } else {
        inDet = qDet->tryGet<dai::ImgDetections>();
    }

    if(inDet) {

        // Draw detections into the rgb image
        detections = inDet->detections;
        draw_detections(detection_img, detections);
    }

    // Copy image data to Bitmap int array
    return cvMatToBmpArray(env, detection_img);
}