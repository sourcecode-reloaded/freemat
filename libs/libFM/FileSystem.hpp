#ifndef __FileSystem_hpp__
#define __FileSystem_hpp__

#include <boost/filesystem.hpp>

namespace FM {
  inline FMString resolve_full_path(const FMString &name) {
    // TODO - add path search in here
    boost::filesystem::path path(name + ".m");
    return absolute(path).c_str();
  }

  inline FMString get_basename_from_full_path(const FMString &name) {
    boost::filesystem::path path(name);
    return path.stem().c_str();
  }
};



#endif
