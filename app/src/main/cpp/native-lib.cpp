#include <chrono>
#include <string>
#include <jni.h>
#include <android/log.h>

#include <libusb/libusb/libusb.h>
#include "depthai/depthai.hpp"

#define LOG_TAG "depthaiAndroid"
#define log(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG, __VA_ARGS__)

using namespace std;
std::shared_ptr<dai::Device> device;
shared_ptr<dai::DataOutputQueue> qRgb, qDepth;

// Closer-in minimum depth, disparity range is doubled (from 95 to 190):
static std::atomic<bool> extended_disparity{true};
auto maxDisparity = extended_disparity ? 190.0f :95.0f;

// Better accuracy for longer distance, fractional disparity 32-levels:
static std::atomic<bool> subpixel{false};
// Better handling for occlusions:
static std::atomic<bool> lr_check{false};


extern "C"
JNIEXPORT void JNICALL
Java_com_example_depthai_1android_1jni_1example_MainActivity_startDevice(JNIEnv *env, jobject thiz,
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
    camRgb->setBoardSocket(dai::CameraBoardSocket::RGB);
    camRgb->setResolution(dai::ColorCameraProperties::SensorResolution::THE_1080_P);
    camRgb->setInterleaved(false);
    camRgb->setColorOrder(dai::ColorCameraProperties::ColorOrder::RGB);

    // Linking
    camRgb->preview.link(xoutRgb->input);

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

    if(oakD) {
        // Output queue will be used to get the rgb frames from the output defined above
        qDepth = device->getOutputQueue("depth", 1, false);
    }

}

extern "C" JNIEXPORT jintArray JNICALL
Java_com_example_depthai_1android_1jni_1example_MainActivity_imageFromJNI(
        JNIEnv* env,
        jobject /* this */) {

    auto inRgb =  qRgb->get<dai::ImgFrame>();
    auto imgData = inRgb->getData();

    uint image_size = inRgb->getHeight()*inRgb->getWidth();

    jintArray result = env->NewIntArray(image_size);
    jint* result_e = env->GetIntArrayElements(result, NULL);

    for (int i = 0; i < image_size; i++)
    {
        int red = imgData[i];
        int green = imgData[i + image_size];
        int blue = imgData[i + image_size*2];

        result_e[i] = 255 << 24 | (red << 16) | (green << 8) | blue;
    }

    env->ReleaseIntArrayElements(result, result_e, NULL);
    return result;
}

extern "C" int colorDisparity(uint8_t disparity)
{
    // Ref: https://www.particleincell.com/2014/colormap

    int r,g,b;

    auto a=(1.0f-(float)disparity/maxDisparity)*5.0f;
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
        result_e[i] = colorDisparity(imgData[i]);
    }

    env->ReleaseIntArrayElements(result, result_e, NULL);
    return result;
}
