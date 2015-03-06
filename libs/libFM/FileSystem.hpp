#ifndef __FileSystem_hpp__
#define __FileSystem_hpp__

#include <boost/filesystem.hpp>

namespace FM {
  inline FMString resolve_full_path(const FMString &name) {
    // TODO - add path search in here
    boost::filesystem::path path(name + ".m");
    return absolute(path).c_str();
  }
};



#endif
