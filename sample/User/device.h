#ifndef __DEVICE__
#define __DEVICE__

#include <NiTE.h>
#include <opencv2/opencv.hpp>

#include <array>

#define USER_COUNT 6

class Device
{
private:
    // Tracker
    nite::UserTracker user_tracker;

    // User Buffer
    nite::UserTrackerFrameRef user_frame;
    cv::Mat user_mat;
    std::array<cv::Vec3b, USER_COUNT> colors;

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

    // Initialize User
    inline void initializeUser();

    // Finalize
    void finalize();

    // Update Data
    void update();

    // Update User
    inline void updateUser();

    // Update Depth
    inline void updateDepth();

    // Draw Data
    void draw();

    // Draw User
    inline void drawUser();

    // Draw Depth
    inline void drawDepth();

    // Show Data
    void show();

    // Show User
    inline void showUser();
};

#endif // __DEVICE__
