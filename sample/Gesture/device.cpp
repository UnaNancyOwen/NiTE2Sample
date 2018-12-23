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

    // Initialize Hand
    initializeHand();
}

// Initialize Hand
inline void Device::initializeHand()
{
    // Create Hand Tracker
    NITE_CHECK( hand_tracker.create() );

    // Start Gesture Detection
    NITE_CHECK( hand_tracker.startGestureDetection( nite::GestureType::GESTURE_WAVE ) );
    NITE_CHECK( hand_tracker.startGestureDetection( nite::GestureType::GESTURE_CLICK ) );
    //NITE_CHECK( hand_tracker.startGestureDetection( nite::GestureType::GESTURE_HAND_RAISE ) ); // Not Recommended
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

    // Draw Gesture
    drawGesture();
}

// Draw Depth
inline void Device::drawDepth()
{
    // Create cv::Mat form Depth Frame
    depth_mat = cv::Mat( depth_height, depth_width, CV_16UC1, const_cast<void*>( depth_frame.getData() ) );
}

// Draw Gesture
inline void Device::drawGesture()
{
    if( depth_mat.empty() ){
        return;
    }

    // Scaling
    depth_mat.convertTo( gesture_mat, CV_8U, -255.0 / 10000.0, 255.0 ); // 0-10000 -> 255(white)-0(black)
    //depth_mat.convertTo( gesture_mat, CV_8U, 255.0 / 10000.0, 0.0 ); // 0-10000 -> 0(black)-255(white)

    // Convert GRAY to BGR
    cv::cvtColor( gesture_mat, gesture_mat, cv::COLOR_GRAY2BGR );

    // Retrieve Gestures
    const nite::Array<nite::GestureData>& gestures = hand_frame.getGestures();

    // Draw Gestures
    uint32_t offset = 0;
    for( int32_t index = 0; index < gestures.getSize(); index++, offset += 20 ){
        // Retrieve Gesture
        const nite::GestureData& gesture = gestures[index];

        // Draw Status
        std::string status = to_string( gesture.getType() );
        if( gesture.isInProgress() ){
            status += " is in progress";
        }
        else if( gesture.isComplete() ){
            status += " is complete";
        }
        else{
            continue;
        }

        cv::putText( gesture_mat, status, cv::Point( 20, 20 + offset ), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Vec3b( 0, 0, 0 ) );
        std::cout << status << std::endl;
    }
}

// Convert Gesture Type to String
inline std::string Device::to_string( nite::GestureType type )
{
    switch( type ){
        case nite::GestureType::GESTURE_WAVE:
            return std::string( "Wave" );
        case nite::GestureType::GESTURE_CLICK:
            return std::string( "Click" );
        case nite::GestureType::GESTURE_HAND_RAISE:
            return std::string( "Hand Raise" );
        default:
            return std::string( "Unknown Gesture" );
    }
}

// Show Data
void Device::show()
{
    // Show Gesture
    showGesture();
}

// Show Gesture
inline void Device::showGesture()
{
    if( gesture_mat.empty() ){
        return;
    }

    // Show Gesture Image
    cv::imshow( "Gesture", gesture_mat );
}
