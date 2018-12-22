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
    #if (DEVICE != REALSENSE)
    // Create User Tracker
    NITE_CHECK( user_tracker.create() );
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

    // Create User Tracker
    NITE_CHECK( user_tracker.create( &device ) );
    #endif
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

    // Update Skeleton
    updateSkeleton();

    // Update Depth
    updateDepth();
}

// Update User
inline void Device::updateUser()
{
    // Update Frame
    NITE_CHECK( user_tracker.readFrame( &user_frame ) );
}

// Update Skeleton
inline void Device::updateSkeleton()
{
    // Retrieve User
    const nite::Array<nite::UserData>& users = user_frame.getUsers();

    // Start Tracking
    #pragma omp parallel for
    for( int32_t i = 0; i < users.getSize(); i++ ){
        const nite::UserData& user = users[i];
        if( user.isNew() ){
            // Start Skeleton Tracking
            user_tracker.startSkeletonTracking( user.getId() );
        }
    }
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

    // Draw Skeleton
    drawSkeleton();
}

// Draw Depth
inline void Device::drawDepth()
{
    // Create cv::Mat form Depth Frame
    depth_mat = cv::Mat( depth_height, depth_width, CV_16UC1, const_cast<void*>( depth_frame.getData() ) );
}

#if (DEVICE == REALSENSE)
// Convert Joint Coordinates To Depth for RealSense
inline nite::Status Device::convertJointCoordinatesToDepth( const float x, const float y, const float z, float * pOutX, float * pOutY )
{
    Rs2PointPixel proj = { 0.0 };
    proj.point[0] = x;
    proj.point[1] = y;
    proj.point[2] = z;

    OPENNI_CHECK( device.invoke( RS2_PROJECT_POINT_TO_PIXEL, reinterpret_cast<void*>( &proj ), static_cast<int>( sizeof( proj ) ) ) );

    *pOutX = proj.pixel[0];
    *pOutY = depth_height - proj.pixel[1];

    return nite::Status::STATUS_OK;
}
#endif

// Draw Skeleton
inline void Device::drawSkeleton()
{
    if( depth_mat.empty() ){
        return;
    }

    // Scaling
    depth_mat.convertTo( skeleton_mat, CV_8U, -255.0 / 10000.0, 255.0 ); // 0-10000 -> 255(white)-0(black)
    //depth_mat.convertTo( skeleton_mat, CV_8U, 255.0 / 10000.0, 0.0 ); // 0-10000 -> 0(black)-255(white)

    // Convert GRAY to BGR
    cv::cvtColor( skeleton_mat, skeleton_mat, cv::COLOR_GRAY2BGR );

    // Retrieve User
    const nite::Array<nite::UserData>& users = user_frame.getUsers();

    // Draw Skeleton Joints
    #pragma omp parallel for
    for( int32_t index = 0; index < users.getSize(); index++ ){
        const nite::UserData& user = users[index];
        if( user.isLost() ){
            continue;
        }

        // Retrieve Skeleton
        const nite::Skeleton& skeleton = user.getSkeleton();
        if( skeleton.getState() != nite::SkeletonState::SKELETON_TRACKED ){
            continue;
        }

        // Draw Joints
        constexpr float threshold = 0.7f;
        for( uint32_t type = 0; type < JOINT_COUNT; type++ ){
            // Retrieve Joint
            const nite::SkeletonJoint& joint = skeleton.getJoint( static_cast< nite::JointType >( type ) );
            if( joint.getPositionConfidence() < threshold ){
                continue;
            }

            // Retrieve Joint Position
            const nite::Point3f& position = joint.getPosition();

            // Convert Joint Coordinates to Depth
            float x, y;
            #if (DEVICE != REALSENSE)
            NITE_CHECK( user_tracker.convertJointCoordinatesToDepth( position.x, position.y, position.z, &x, &y ) ); // for PrimeSensor
            #else
            NITE_CHECK( convertJointCoordinatesToDepth( position.x, position.y, position.z, &x, &y ) ); // for RealSense
            #endif

            // Draw Joint
            const uint32_t depth_x = static_cast<uint32_t>( x );
            const uint32_t depth_y = static_cast<uint32_t>( y );
            if( 0 <= depth_x && depth_x < depth_width && 0 <= depth_y && depth_y < depth_height ){
                const cv::Point point( depth_x, depth_y );
                cv::circle( skeleton_mat, point, 5, colors[index], -1 );
            }
        }

        /*
        // Retrieve Bounding Box
        const nite::BoundingBox& bounding_box = user.getBoundingBox();

        // Draw Bouding Box
        const cv::Point point_min( static_cast<uint32_t>( bounding_box.min.x ), static_cast<uint32_t>( bounding_box.min.y ) );
        const cv::Point point_max( static_cast<uint32_t>( bounding_box.max.x ), static_cast<uint32_t>( bounding_box.max.y ) );
        cv::rectangle( skeleton_mat, point_min, point_max, colors[index], 1 );
        */
    }
}

// Show Data
void Device::show()
{
    // Show Skeleton
    showSkeleton();
}

// Show Skeleton
inline void Device::showSkeleton()
{
    if( skeleton_mat.empty() ){
        return;
    }

    // Show Skeleton Image
    cv::imshow( "Skeleton", skeleton_mat );
}
