#pragma once

// std
#include <vector>

// project
#include "NeuralNetworkProperties.hpp"
#include "depthai-shared/common/DetectionNetworkType.hpp"
#include "depthai-shared/common/optional.hpp"

namespace dai {

/**
 *  Specify properties for DetectionNetwork
 */
struct DetectionNetworkProperties : PropertiesSerializable<NeuralNetworkProperties, DetectionNetworkProperties> {
    /// Generic Neural Network properties
    DetectionNetworkType nnFamily;
    float confidenceThreshold;

    /// YOLO specific network properties
    int classes;
    int coordinates;
    std::vector<float> anchors;
    std::map<std::string, std::vector<int>> anchorMasks;
    float iouThreshold;
};

DEPTHAI_SERIALIZE_EXT(DetectionNetworkProperties,
                      nnFamily,
                      blobSize,
                      blobUri,
                      numFrames,
                      numThreads,
                      numNCEPerThread,
                      confidenceThreshold,
                      classes,
                      coordinates,
                      anchors,
                      anchorMasks,
                      iouThreshold);

}  // namespace dai
