#include "device.h"
#include "util.h"

// Constructor
Device::Device()
{
    // Initialize
    initialize();
}

// Destructor
Device::~Device()
{
    // Finalize
    finalize();
}

// Processing
void Device::run()
{
    // Main Loop
    while( true ){
        // Update Data
        update();

        // Draw Data
        draw();

        // Show Data
        show();

        // Key Check
        const int32_t key = cv::waitKey( 10 );
        if( key == 'q' ){
            break;
        }
    }
}

// Initialize
void Device::initialize()
{
    cv::setUseOptimized( true );

    // Initiaize Nite2
    NITE_CHECK( nite::NiTE::initialize() );

    // Initialize User
    initializeUser();

    // Initalize Color Table for Visualization
    colors[0] = cv::Vec3b( 255,   0,   0 ); // Blue
    colors[1] = cv::Vec3b(   0, 255,   0 ); // Green
    colors[2] = cv::Vec3b(   0,   0, 255 ); // Red
    colors[3] = cv::Vec3b( 255, 255,   0 ); // Cyan
    colors[4] = cv::Vec3b( 255,   0, 255 ); // Magenta
    colors[5] = cv::Vec3b(   0, 255, 255 ); // Yellow
}

// Initialize User
inline void Device::initializeUser()
{
    // Create User Tracker
    NITE_CHECK( user_tracker.create() );
}

// Finalize
void Device::finalize()
{
    // Close Windows
    cv::destroyAllWindows();
}

// Update Data
void Device::update()
{
    // Update User
    updateUser();

    // Update Depth
    updateDepth();
}

// Update User
inline void Device::updateUser()
{
    // Update Frame
    NITE_CHECK( user_tracker.readFrame( &user_frame ) );
}

// Update Depth
inline void Device::updateDepth()
{
    // Retrieve Frame
    depth_frame = user_frame.getDepthFrame();
    if( !depth_frame.isValid() ){
        throw std::runtime_error( "failed can not retrieve depth frame" );
        std::exit( EXIT_FAILURE );
    }

    // Retrive Frame Size
    depth_width = depth_frame.getWidth();
    depth_height = depth_frame.getHeight();
}

// Draw Data
void Device::draw()
{
    // Draw Depth
    drawDepth();

    // Draw User
    drawUser();
}

// Draw Depth
inline void Device::drawDepth()
{
    // Create cv::Mat form Depth Frame
    depth_mat = cv::Mat( depth_height, depth_width, CV_16UC1, const_cast<void*>( depth_frame.getData() ) );
}

// Draw User
inline void Device::drawUser()
{
    if( depth_mat.empty() ){
        return;
    }

    // Scaling
    depth_mat.convertTo( user_mat, CV_8U, -255.0 / 10000.0, 255.0 ); // 0-10000 -> 255(white)-0(black)
    //depth_mat.convertTo( user_mat, CV_8U, 255.0 / 10000.0, 0.0 ); // 0-10000 -> 0(black)-255(white)

    // Convert GRAY to BGR
    cv::cvtColor( user_mat, user_mat, cv::COLOR_GRAY2BGR );

    // Retrieve User Map
    const nite::UserMap& user_map = user_frame.getUserMap();

    // Draw User Area
    const nite::UserId* user_id = user_map.getPixels();
    user_mat.forEach<cv::Vec3b>( [&]( cv::Vec3b& p, const int* position ){
        const uint32_t index = position[0] * depth_width + position[1];
        const uint16_t id    = user_id[index];
        if( id != 0 && id <= USER_COUNT ){
            p = colors[id - 1];
        }
    } );
}

// Show Data
void Device::show()
{
    // Show User
    showUser();
}

// Show User
inline void Device::showUser()
{
    if( user_mat.empty() ){
        return;
    }

    // Show User Image
    cv::imshow( "User", user_mat );
}
