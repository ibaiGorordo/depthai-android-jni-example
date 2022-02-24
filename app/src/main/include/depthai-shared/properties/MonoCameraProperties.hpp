#pragma once

#include "depthai-shared/common/CameraBoardSocket.hpp"
#include "depthai-shared/common/CameraImageOrientation.hpp"
#include "depthai-shared/datatype/RawCameraControl.hpp"
#include "depthai-shared/properties/Properties.hpp"

namespace dai {

/**
 * Specify properties for MonoCamera such as camera ID, ...
 */
struct MonoCameraProperties : PropertiesSerializable<Properties, MonoCameraProperties> {
    /**
     * Select the camera sensor resolution: 1280×720, 1280×800, 640×400, 640×480
     */
    enum class SensorResolution : int32_t { THE_720_P, THE_800_P, THE_400_P, THE_480_P };

    /*
     * Initial controls applied to MonoCamera node
     */
    RawCameraControl initialControl;

    /**
     * Which socket will mono camera use
     */
    CameraBoardSocket boardSocket = CameraBoardSocket::AUTO;

    /**
     * Camera sensor image orientation / pixel readout
     */
    CameraImageOrientation imageOrientation = CameraImageOrientation::AUTO;

    /**
     * Select the camera sensor resolution
     */
    SensorResolution resolution = SensorResolution::THE_720_P;
    /**
     * Camera sensor FPS
     */
    float fps = 30.0;
};

DEPTHAI_SERIALIZE_EXT(MonoCameraProperties, initialControl, boardSocket, imageOrientation, resolution, fps);

}  // namespace dai
