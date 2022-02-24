#pragma once
#include <cstdint>
namespace dai {
/**
 * Which Camera socket to use.
 *
 * AUTO denotes that the decision will be made by device
 */
enum class CameraBoardSocket : int32_t { AUTO = -1, RGB, LEFT, RIGHT };

}  // namespace dai
