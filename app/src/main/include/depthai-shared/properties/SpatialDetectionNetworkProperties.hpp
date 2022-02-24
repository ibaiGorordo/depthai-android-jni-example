#pragma once

// std
#include <vector>

// libraries

// project
#include "DetectionNetworkProperties.hpp"
#include "depthai-shared/common/DetectionNetworkType.hpp"
#include "depthai-shared/common/optional.hpp"
#include "depthai-shared/datatype/RawSpatialLocationCalculatorConfig.hpp"

namespace dai {

/**
 * Specify properties for SpatialDetectionNetwork
 */
struct SpatialDetectionNetworkProperties : PropertiesSerializable<DetectionNetworkProperties, SpatialDetectionNetworkProperties> {
    float detectedBBScaleFactor = 1.0;
    SpatialLocationCalculatorConfigThresholds depthThresholds;
    SpatialLocationCalculatorAlgorithm calculationAlgorithm = SpatialLocationCalculatorAlgorithm::AVERAGE;
};

DEPTHAI_SERIALIZE_EXT(SpatialDetectionNetworkProperties,
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
                      iouThreshold,
                      detectedBBScaleFactor,
                      depthThresholds,
                      calculationAlgorithm);

}  // namespace dai
