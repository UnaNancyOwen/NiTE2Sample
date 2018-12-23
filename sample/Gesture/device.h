#ifndef __DEVICE__
#define __DEVICE__

#include <NiTE.h>
#include <opencv2/opencv.hpp>

#include <array>

class Device
{
private:
    // Tracker
    nite::HandTracker hand_tracker;

    // Hand Buffer
    nite::HandTrackerFrameRef hand_frame;
    cv::Mat gesture_mat;

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

    // Draw Gesture
    inline void drawGesture();

    // Draw Depth
    inline void drawDepth();

    // Convert Gesture Type to String
    inline std::string to_string( nite::GestureType type );

    // Show Data
    void show();

    // Show Gesture
    inline void showGesture();
};

#endif // __DEVICE__
