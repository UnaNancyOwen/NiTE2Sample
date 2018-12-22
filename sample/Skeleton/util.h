#ifndef __UTIL__
#define __UTIL__

#include <OpenNI.h>
#include <NiTE.h>

#include <sstream>
#include <stdexcept>

// Error Check Macro
#define OPENNI_CHECK( ret )                                       \
    if( ret != openni::Status::STATUS_OK ){                       \
        std::stringstream ss;                                     \
        ss << "failed " #ret " " << std::hex << ret << std::endl; \
        throw std::runtime_error( ss.str().c_str() );             \
    }

#define NITE_CHECK( ret )                                         \
    if( ret != nite::Status::STATUS_OK ){                         \
        std::stringstream ss;                                     \
        ss << "failed " #ret " " << std::hex << ret << std::endl; \
        throw std::runtime_error( ss.str().c_str() );             \
    }

#endif  // __UTIL__
