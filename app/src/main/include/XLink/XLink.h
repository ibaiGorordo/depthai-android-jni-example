// Copyright (C) 2018-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

///
/// @file
/// @brief     Application configuration Leon header
///

#ifndef _XLINK_H
#define _XLINK_H
#include "XLinkPublicDefines.h"

#ifdef __cplusplus
extern "C"
{
#endif

// ------------------------------------
// Device management. Begin.
// ------------------------------------

/**
 * @brief Initializes XLink and scheduler
 * @param globalHandler[in] XLink global communication parameters
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 * Now XLink can work with PCIe and USB simultaneously.
 */
XLinkError_t XLinkInitialize(XLinkGlobalHandler_t* globalHandler);

#ifndef __DEVICE__

/**
 * @brief Checks consistency of device description
 * @param[in]   in_deviceDesc - structure with device requirements (protocol, platform).
 * @param[in]   state - state of device enum (booted, not booted or any state)
 * @return Result of checking: (1) for a correct description, otherwise (0)
 */
int XLinkIsDescriptionValid(const deviceDesc_t *in_deviceDesc, const XLinkDeviceState_t state);

/**
 * @brief Returns Myriad device description which meets the requirements
 * @param[in]   in_deviceRequirements - structure with device requirements (protocol, platform).
 * @note        If in_deviceRequirements has device name specified,
 *                  this function tries to get a device with that exact name
 *                  and fails if such device is unavailable
 * @param[out]  out_foundDevice - found device description
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */
XLinkError_t XLinkFindFirstSuitableDevice(const deviceDesc_t in_deviceRequirements,
                                          deviceDesc_t *out_foundDevice);

/**
 * @brief Returns all Myriad devices description which meets the requirements
 * @param[in]      state - state of device enum (booted, not booted or any state)
 * @param[in]      in_deviceRequirements - structure with device requirements (protocol, platform).
 * @param[in,out]  out_foundDevicesPtr - pointer to array with all found devices descriptions
 * @param[out]     devicesArraySize - size of out_foundDevicesPtr
 * @param[out]     out_foundDevicesCount - amount of found devices
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */
XLinkError_t XLinkFindAllSuitableDevices(const deviceDesc_t in_deviceRequirements,
                                         deviceDesc_t *out_foundDevicesPtr,
                                         const unsigned int devicesArraySize,
                                         unsigned int *out_foundDevicesCount);

/**
 * @brief Connects to specific device, starts dispatcher and pings remote
 * @param[in,out] handler - XLink communication parameters (file path name for underlying layer)
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */
XLinkError_t XLinkConnect(XLinkHandler_t* handler);

/**
 * @brief Puts device into bootloader mode
 * @param deviceDesc - device description structure, obtained from XLinkFind* functions call
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */
XLinkError_t XLinkBootBootloader(const deviceDesc_t* deviceDesc);

/**
 * @brief Boots firmware binary from memory to the remote device
 * @param deviceDesc - device description structure, obtained from XLinkFind* functions call
 * @param buffer - mvcmd file contents
 * @param size - size of buffer
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */
XLinkError_t XLinkBootMemory(const deviceDesc_t* deviceDesc, const uint8_t* buffer, unsigned long size);

/**
 * @brief Boots specified firmware binary to the remote device
 * @param deviceDesc - device description structure, obtained from XLinkFind* functions call
 * @param binaryPath - path to the *.mvcmd file
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */
XLinkError_t XLinkBoot(const deviceDesc_t* deviceDesc, const char* binaryPath);

/**
 * @brief Boots specified firmware binary to the remote device
 * @param deviceDesc - device description structure, obtained from XLinkFind* functions call
 * @param firmware - firmware buffer
 * @param length - firmware buffer length
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */
XLinkError_t XLinkBootFirmware(const deviceDesc_t* deviceDesc, const char* firmware, unsigned long length);

/**
 * @brief Resets the remote device and close all open local handles for this device
 * @warning This function should be used in a host application
 * @param[in] id - link Id obtained from XLinkConnect in the handler parameter
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */

XLinkError_t XLinkResetRemote(linkId_t id);

/**
 * @brief Resets the remote device and close all open local handles for this device
 * @warning This function should be used in a host application
 * @param[in] id - link Id obtained from XLinkConnect in the handler parameter
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */

XLinkError_t XLinkResetRemoteTimeout(linkId_t id, int timeoutMs);

/**
 * @brief Closes all and release all memory
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */
XLinkError_t XLinkResetAll();

/**
 * @brief Retrieves USB speed of certain connected device
 * @return UsbSpeed_t enum describing the usb connection speed
 */
UsbSpeed_t XLinkGetUSBSpeed(linkId_t id);

/**
 * @brief Returns mx serial of current connected device
 * @return pointer to mx serial string
 */
const char* XLinkGetMxSerial(linkId_t id);

/**
 * @brief Returns enum string value
 * @return Pointer to null terminated string
 */
const char* XLinkErrorToStr(XLinkError_t val);

/**
 * @brief Returns enum string value
 * @return Pointer to null terminated string
 */
const char* XLinkProtocolToStr(XLinkProtocol_t val);

/**
 * @brief Returns enum string value
 * @return Pointer to null terminated string
 */
const char* XLinkPlatformToStr(XLinkPlatform_t val);

/**
 * @brief Returns enum string value
 * @return Pointer to null terminated string
 */
const char* XLinkDeviceStateToStr(XLinkDeviceState_t val);

/**
 * @brief Returns enum string value
 * @return Pointer to null terminated string
 */
const char* XLinkPCIEBootloaderToStr(XLinkPCIEBootloader val);

#endif // __DEVICE__

/**
 * @brief Profiling funcs - keeping them global for now
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */
XLinkError_t XLinkProfStart();
XLinkError_t XLinkProfStop();
XLinkError_t XLinkProfPrint();


// ------------------------------------
// Device management. End.
// ------------------------------------




// ------------------------------------
// Device streams management. Begin.
// ------------------------------------

/**
 * @brief Opens a stream in the remote that can be written to by the local
 *        Allocates stream_write_size (aligned up to 64 bytes) for that stream
 * @param[in] id - link Id obtained from XLinkConnect in the handler parameter
 * @param[in] name - stream name
 * @param[in] stream_write_size - stream buffer size
 * @return Link Id: INVALID_STREAM_ID for failure
 */
streamId_t XLinkOpenStream(linkId_t id, const char* name, int stream_write_size);

/**
 * @brief Closes stream for any further data transfer
 *        Stream will be deallocated when all pending data has been released
 * @param[in] streamId - link Id obtained from XLinkOpenStream call
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */
XLinkError_t XLinkCloseStream(streamId_t const streamId);

/**
 * @brief Sends a package to initiate the writing of data to a remote stream
 * @warning Actual size of the written data is ALIGN_UP(size, 64)
 * @param[in] streamId - stream link Id obtained from XLinkOpenStream call
 * @param[in] buffer - data buffer to be transmitted
 * @param[in] size - size of the data to be transmitted
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */
XLinkError_t XLinkWriteData(streamId_t const streamId, const uint8_t* buffer, int size);

/**
 * @brief Sends a package to initiate the writing of data to a remote stream
 * @warning Actual size of the written data is ALIGN_UP(size, 64)
 * @param[in] streamId – stream link Id obtained from XLinkOpenStream call
 * @param[in] buffer – data buffer to be transmitted
 * @param[in] size – size of the data to be transmitted
 * @param[in] msTimeout – time in milliseconds after which operation times out
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success,  X_LINK_TIMEOUT when msTimeout time passes
 */
XLinkError_t XLinkWriteDataWithTimeout(streamId_t const streamId, const uint8_t* buffer, int size, unsigned int msTimeout);

/**
 * @brief Reads data from local stream. Will only have something if it was written to by the remote
 * @param[in]   streamId - stream link Id obtained from XLinkOpenStream call
 * @param[out]  packet - structure containing output data buffer and received size
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */
XLinkError_t XLinkReadData(streamId_t const streamId, streamPacketDesc_t** packet);

/**
 * @brief Reads data from local stream. Will only have something if it was written to by the remote
 * @param[in]   streamId – stream link Id obtained from XLinkOpenStream call
 * @param[out]  packet – structure containing output data buffer and received size
 * @param[in]   msTimeout – time in milliseconds after which operation times out
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success, X_LINK_TIMEOUT when msTimeout time passes
 */
XLinkError_t XLinkReadDataWithTimeout(streamId_t const streamId, streamPacketDesc_t** packet, unsigned int msTimeout);

/**
 * @brief Releases specific data from stream
 * @param[in] streamId – stream link Id obtained from XLinkOpenStream call
 * @param[in] packetId – ID of the package to be released from the stream
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */
XLinkError_t XLinkReleaseSpecificData(streamId_t streamId, streamPacketDesc_t* packetDesc);

/**
 * @brief Reads data from local stream and moves ownership. Will only have something if it was written to by the remote
 * @note Caller is responsible for deallocating with XLinkDeallocateMoveData(streamPacketDesc_t::data, streamPacketDesc_t::length)
 * @param[in]   streamId - stream link Id obtained from XLinkOpenStream call
 * @param[out]  packet - structure containing output data buffer and received size
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */
XLinkError_t XLinkReadMoveData(streamId_t const streamId, streamPacketDesc_t* const packet);

/**
 * @brief Reads data from local stream and moves ownership. Will only have something if it was written to by the remote
 * @note Caller is responsible for deallocating with XLinkDeallocateMoveData(streamPacketDesc_t::data, streamPacketDesc_t::length)
 * @param[in]   streamId - stream link Id obtained from XLinkOpenStream call
 * @param[out]  packet - structure containing output data buffer and received size
 * @param[in]   msTimeout – time in milliseconds after which operation times out
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success, X_LINK_TIMEOUT when msTimeout time passes
 */
XLinkError_t XLinkReadMoveDataWithTimeout(streamId_t const streamId, streamPacketDesc_t *const packet, const unsigned int msTimeout);

/**
 * @brief Deallocate memory within streamPacketDesc_t received from a previous call to XLinkReadMoveData() or XLinkReadMoveDataWithTimeout()
 * @param[in]  data - streamPacketDesc_t::data pointer
 * @param[in]  length - streamPacketDesc_t::length
 */
void XLinkDeallocateMoveData(void* const data, const uint32_t length);

/**
 * @brief Releases data from stream - This should be called after the data obtained from
 *  XlinkReadData is processed
 * @param[in] streamId - stream link Id obtained from XLinkOpenStream call
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */
XLinkError_t XLinkReleaseData(streamId_t const streamId);

/**
 * @brief Reads data from local stream with timeout in ms. Will only have something if it was written to by the remote.
 * Limitations.
 *      If we reached timeout and the event waiter returned timeout error code
 *      there potentially will be a time frame when XLink still has not marked the read
 *      event as completed with timeout state but the event receiver has just received data
 *      from another end and match that data with the read event. In this case we can lose
 *      data. Should be fixed for the next release.
 * @param[in]   streamId – stream link Id obtained from XLinkOpenStream call
 * @param[out]  packet – structure containing output data buffer and received size
 * @param[in]   timeoutMs – timeout for a read operation in milliseconds
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */
XLinkError_t XLinkReadDataWithTimeout(streamId_t streamId, streamPacketDesc_t** packet, unsigned int timeoutMs);

/**
 * @brief Sends a package to initiate the writing of data to a remote stream with timeout in ms
 * XLinkWriteDataWithTimeout is not fully supported yet. The XLinkWriteData method is called instead.
 * @warning Actual size of the written data is ALIGN_UP(size, 64)
 * @param[in] streamId – stream link Id obtained from XLinkOpenStream call
 * @param[in] buffer – data buffer to be transmitted
 * @param[in] size – size of the data to be transmitted
 * @param[in] timeoutMs – timeout for a write operation in milliseconds
 * @return Status code of the operation: X_LINK_SUCCESS (0) for success
 */
XLinkError_t XLinkWriteDataWithTimeout(streamId_t streamId, const uint8_t* buffer, int size, unsigned int timeoutMs);

// ------------------------------------
// Device streams management. End.
// ------------------------------------




// ------------------------------------
// Deprecated API. Begin.
// ------------------------------------

#ifndef __DEVICE__

XLinkError_t XLinkGetDeviceName(int index, char* name, int nameSize);
XLinkError_t XLinkGetDeviceNameExtended(int index, char* name, int nameSize, int pid);

XLinkError_t XLinkBootRemote(const char* deviceName, const char* binaryPath);
XLinkError_t XLinkDisconnect(linkId_t id);

XLinkError_t XLinkGetAvailableStreams(linkId_t id);

XLinkError_t XLinkAsyncWriteData();

XLinkError_t XLinkSetDeviceOpenTimeOutMsec(unsigned int msec);
XLinkError_t XLinkSetCommonTimeOutMsec(unsigned int msec);

// unsafe
XLinkError_t XLinkGetFillLevel(streamId_t const streamId, int isRemote, int* fillLevel);

#endif // __DEVICE__

// ------------------------------------
// Deprecated API. End.
// ------------------------------------

#ifdef __cplusplus
}
#endif

#endif
