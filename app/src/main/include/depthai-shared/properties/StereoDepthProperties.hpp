#pragma once

#include "depthai-shared/common/CameraBoardSocket.hpp"
#include "depthai-shared/common/EepromData.hpp"
#include "depthai-shared/common/optional.hpp"
#include "depthai-shared/datatype/RawStereoDepthConfig.hpp"
#include "depthai-shared/properties/Properties.hpp"

namespace dai {

/**
 * Specify properties for StereoDepth
 */
struct StereoDepthProperties : PropertiesSerializable<Properties, StereoDepthProperties> {
    static constexpr const std::int32_t AUTO = -1;

    struct RectificationMesh {
        /**
         * Uri which points to the mesh array for 'left' input rectification
         */
        std::string meshLeftUri;
        /**
         * Uri which points to the mesh array for 'right' input rectification
         */
        std::string meshRightUri;
        /**
         * Mesh array size in bytes, for each of 'left' and 'right' (need to match)
         */
        tl::optional<std::uint32_t> meshSize;
        /**
         * Distance between mesh points, in the horizontal direction
         */
        uint16_t stepWidth = 16;
        /**
         * Distance between mesh points, in the vertical direction
         */
        uint16_t stepHeight = 16;

        DEPTHAI_SERIALIZE(RectificationMesh, meshLeftUri, meshRightUri, meshSize, stepWidth, stepHeight);
    };

    /// Initial stereo config
    RawStereoDepthConfig initialConfig;

    using MedianFilter = dai::MedianFilter;

    using DepthAlign = dai::RawStereoDepthConfig::AlgorithmControl::DepthAlign;

    /**
     * Which camera to align disparity/depth to.
     * When configured (not AUTO), takes precedence over 'depthAlign'
     */
    CameraBoardSocket depthAlignCamera = CameraBoardSocket::AUTO;

    bool enableRectification = true;

    /**
     * Fill color for missing data at frame edges - grayscale 0..255, or -1 to replicate pixels
     */
    std::int32_t rectifyEdgeFillColor = -1;
    /**
     * Input frame width. Optional (taken from MonoCamera nodes if they exist)
     */
    tl::optional<std::int32_t> width;
    /**
     * Input frame height. Optional (taken from MonoCamera nodes if they exist)
     */
    tl::optional<std::int32_t> height;
    /**
     * Output disparity/depth width. Currently only used when aligning to RGB
     */
    tl::optional<std::int32_t> outWidth;
    /**
     * Output disparity/depth height. Currently only used when aligning to RGB
     */
    tl::optional<std::int32_t> outHeight;
    /**
     * Whether to keep aspect ratio of the input (rectified) or not
     */
    bool outKeepAspectRatio = true;

    /**
     * Specify a direct warp mesh to be used for rectification,
     * instead of intrinsics + extrinsic matrices
     */
    RectificationMesh mesh;

    /**
     * Whether to enable switching stereo modes at runtime or not.
     * E.g. standard to subpixel, standard+LR-check to subpixel + LR-check.
     * Note: It will allocate resources for worst cases scenario,
     * should be enabled only if dynamic mode switch is required.
     * Default value: false.
     */
    bool enableRuntimeStereoModeSwitch = false;

    /// Num frames in output pool
    int numFramesPool = 3;

    /**
     * Number of shaves reserved for stereo depth post processing.
     * Post processing can use multiple shaves to increase performance.
     * -1 means auto, resources will be allocated based on enabled filters.
     * 0 means that it will reuse the shave assigned for main stereo algorithm.
     * For optimal performance it's recommended to allocate more than 0,
     * so post processing will run in parallel with main stereo algorithm.
     */
    std::int32_t numPostProcessingShaves = AUTO;

    /**
     * Number of memory slices reserved for stereo depth post processing.
     * -1 means auto, memory will be allocated based on initial stereo settings and number of shaves.
     * 0 means that it will reuse the memory slices assigned for main stereo algorithm.
     * For optimal performance it's recommended to allocate more than 0,
     * so post processing will run in parallel with main stereo algorithm.
     */
    std::int32_t numPostProcessingMemorySlices = AUTO;

    /**
     * Whether to use focal length from calibration intrinsics or calculate based on calibration FOV.
     * Default behaviour is AUTO, for fisheye lenses focal length is taken from calibration intrinsics,
     * otherwise calculated from FOV and image resolution: focalLength = calib.width / (2.f * tan(calib.fov / 2 / 180.f * pi));
     */
    tl::optional<bool> focalLengthFromCalibration;
};

DEPTHAI_SERIALIZE_EXT(StereoDepthProperties,
                      initialConfig,
                      depthAlignCamera,
                      enableRectification,
                      rectifyEdgeFillColor,
                      width,
                      height,
                      outWidth,
                      outHeight,
                      outKeepAspectRatio,
                      mesh,
                      enableRuntimeStereoModeSwitch,
                      numFramesPool,
                      numPostProcessingShaves,
                      numPostProcessingMemorySlices,
                      focalLengthFromCalibration);

}  // namespace dai
