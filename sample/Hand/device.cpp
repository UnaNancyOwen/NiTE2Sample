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

    // Initialize OpenNI2
    OPENNI_CHECK( openni::OpenNI::initialize() );

    // Initiaize Nite2
    NITE_CHECK( nite::NiTE::initialize() );

    // Initialize Hand
    initializeHand();

    // Initalize Color Table for Visualization
    colors[0]  = cv::Vec3b( 255,   0,   0 ); // Blue
    colors[1]  = cv::Vec3b(   0, 255,   0 ); // Green
    colors[2]  = cv::Vec3b(   0,   0, 255 ); // Red
    colors[3]  = cv::Vec3b( 255, 255,   0 ); // Cyan
    colors[4]  = cv::Vec3b( 255,   0, 255 ); // Magenta
    colors[5]  = cv::Vec3b(   0, 255, 255 ); // Yellow
}

// Initialize Hand
inline void Device::initializeHand()
{
    #if (DEVICE != REALSENSE)
    // Create Hand Tracker
    NITE_CHECK( hand_tracker.create() );
    #else
    // Retrive Connected Devices List
    openni::Array<openni::DeviceInfo> device_info_list;
    openni::OpenNI::enumerateDevices( &device_info_list );
    if( !device_info_list.getSize() ){
        throw std::runtime_error( "failed could not find devices" );
        std::exit( EXIT_FAILURE );
    }

    // Open Device
    const openni::DeviceInfo& device_info = device_info_list[0];
    const std::string device_uri = device_info.getUri();
    OPENNI_CHECK( device.open( device_uri.c_str() ) );

    // Create Hand Tracker
    NITE_CHECK( hand_tracker.create( &device ) );
    #endif

    // Start Gesture Detection
    NITE_CHECK( hand_tracker.startGestureDetection( nite::GestureType::GESTURE_CLICK ) );
    NITE_CHECK( hand_tracker.startGestureDetection( nite::GestureType::GESTURE_WAVE ) );
    NITE_CHECK( hand_tracker.startGestureDetection( nite::GestureType::GESTURE_HAND_RAISE ) );
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
    // Update Hand
    updateHand();

    // Update Depth
    updateDepth();
}

// Update Hnad
inline void Device::updateHand()
{
    // Update Frame
    NITE_CHECK( hand_tracker.readFrame( &hand_frame ) );

    // Retrieve Gestures
    const nite::Array<nite::GestureData>& gestures = hand_frame.getGestures();

    // Start Hand Tracking with Gesture Detected Position
    #pragma omp parallel for
    for( int32_t index = 0; index < gestures.getSize(); index++ ){
        // Retrieve Gesture
        const nite::GestureData& gesture = gestures[index];

        if( gesture.isComplete() ){
            // Retrieve Current Position
            const nite::Point3f& position = gesture.getCurrentPosition();

            // Start Hand Tracking
            nite::HandId hand_id;
            const nite::Status status = hand_tracker.startHandTracking( position, &hand_id );
            if( status == nite::Status::STATUS_OK ){
                std::cout << "Start Hand Tracking (" << hand_id << ")" << std::endl;
            }
        }
    }
}

// Update Depth
inline void Device::updateDepth()
{
    // Retrieve Frame
    depth_frame = hand_frame.getDepthFrame();
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

    // Draw Hand
    drawHand();
}

// Draw Depth
inline void Device::drawDepth()
{
    // Create cv::Mat form Depth Frame
    depth_mat = cv::Mat( depth_height, depth_width, CV_16UC1, const_cast<void*>( depth_frame.getData() ) );
}

// Draw Hand
inline void Device::drawHand()
{
    if( depth_mat.empty() ){
        return;
    }

    // Scaling
    depth_mat.convertTo( hand_mat, CV_8U, -255.0 / 10000.0, 255.0 ); // 0-10000 -> 255(white)-0(black)
    //depth_mat.convertTo( hand_mat, CV_8U, 255.0 / 10000.0, 0.0 ); // 0-10000 -> 0(black)-255(white)

    // Convert GRAY to BGR
    cv::cvtColor( hand_mat, hand_mat, cv::COLOR_GRAY2BGR );

    // Retrieve Hands
    const nite::Array<nite::HandData>& hands = hand_frame.getHands();

    // Draw Hands
    #pragma omp parallel for
    for( int32_t index = 0; index < hands.getSize(); index++ ){
        // Retrieve Hand
        const nite::HandData& hand = hands[index];

        // Check Status
        if( !hand.isTracking() ){
            continue;
        }

        // Retrieve Position
        const nite::Point3f& position = hand.getPosition();
        // Convert Joint Coordinates to Depth
        float x, y;
        #if (DEVICE != REALSENSE)
        NITE_CHECK( hand_tracker.convertHandCoordinatesToDepth( position.x, position.y, position.z, &x, &y ) ); // for PrimeSensor
        #else
        NITE_CHECK( convertJointCoordinatesToDepth( position.x, position.y, position.z, &x, &y ) ); // for RealSense
        #endif

        // Draw Hand
        const uint32_t depth_x = static_cast<uint32_t>( x );
        const uint32_t depth_y = static_cast<uint32_t>( y );
        if( 0 <= depth_x && depth_x < depth_width && 0 <= depth_y && depth_y < depth_height ){
            const cv::Point point( depth_x, depth_y );
            cv::circle( hand_mat, point, 30, colors[hand.getId() % HAND_COUNT], 2 );
        }
    }
}

#if (DEVICE == REALSENSE)
// Convert Joint Coordinates To Depth for RealSense
inline nite::Status Device::convertJointCoordinatesToDepth( const float x, const float y, const float z, float * pOutX, float * pOutY )
{
    Rs2PointPixel proj = { 0.0 };
    proj.point[0] = x;
    proj.point[1] = y;
    proj.point[2] = z;

    OPENNI_CHECK( device.invoke( RS2_PROJECT_POINT_TO_PIXEL, reinterpret_cast< void* >( &proj ), static_cast< int >( sizeof( proj ) ) ) );

    *pOutX = proj.pixel[0];
    *pOutY = depth_height - proj.pixel[1];

    return nite::Status::STATUS_OK;
}
#endif

// Show Data
void Device::show()
{
    // Show Hand
    showHand();
}

// Show Hand
inline void Device::showHand()
{
    if( hand_mat.empty() ){
        return;
    }

    // Show Hand Image
    cv::imshow( "Hand", hand_mat );
}
