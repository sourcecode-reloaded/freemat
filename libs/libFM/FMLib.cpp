#include "FMLib.hpp"

#ifdef WIN32
#include <Windows.h>
#include <ShlObj.h>
#endif

bool FMFile::open(FMString mode)
{
	_fp = fopen(_name.c_str(), mode.c_str());
	return (_fp != NULL);
}

FMString FMFile::errorString() const
{
	return FMString(strerror(errno));
}


FMString FMDir::homePath() {
    //TODO - Fix fir windows
#ifndef WIN32
    return FMString(getenv("HOME"));
#else
	  CHAR path[MAX_PATH];
	  if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, path)))
		  return FMString(path);
	  return FMString("");
#endif
}
