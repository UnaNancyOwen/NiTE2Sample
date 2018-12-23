#ifndef __DEVICE__
#define __DEVICE__

#include <OpenNI.h>
#include <NiTE.h>
#include <opencv2/opencv.hpp>

#include <array>

#define HAND_COUNT 6

// Specify Device
// For RealSense https://github.com/IntelRealSense/librealsense/issues/2825
#define PRIMESENSOR 0
#define REALSENSE 1
#define DEVICE PRIMESENSOR

#if (DEVICE == REALSENSE)
#define RS2_PROJECT_POINT_TO_PIXEL 0x1000
struct Rs2PointPixel
{
    float point[3];
    float pixel[2];
};
#endif

class Device
{
private:
    // Device
    openni::Device device;

    // Tracker
    nite::HandTracker hand_tracker;

    // Hand Buffer
    nite::HandTrackerFrameRef hand_frame;
    cv::Mat hand_mat;
    std::array<cv::Vec3b, HAND_COUNT> colors;

    // Depth Buffer
    openni::VideoFrameRef depth_frame;
    cv::Mat depth_mat;
    uint32_t depth_width = 640;
    uint32_t depth_height = 480;
    uint32_t depth_fps = 30;

public:
    // Constructor
    Device();

    // Destructor
    ~Device();

    // Processing
    void run();

private:
    // Initialize
    void initialize();

    // Initialize Hand
    inline void initializeHand();

    // Finalize
    void finalize();

    // Update Data
    void update();

    // Update Hand
    inline void updateHand();

    // Update Depth
    inline void updateDepth();

    // Draw Data
    void draw();

    // Draw Hand
    inline void drawHand();

    // Draw Depth
    inline void drawDepth();

    #if (DEVICE == REALSENSE)
    // Convert Joint Coordinates to Depth for RealSense
    inline nite::Status convertJointCoordinatesToDepth( const float x, const float y, const float z, float* pOutX, float* pOutY );
    #endif

    // Show Data
    void show();

    // Show Hand
    inline void showHand();
};

#endif // __DEVICE__
