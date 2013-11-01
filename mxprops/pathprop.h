#pragma once
#include <string>

struct PathPropData
{
  bool isPathType;
  bool isRelative;
  std::string originalPath;
  std::string originalFilePath;

  PathPropData()
  : isPathType(false),
    isRelative(false)
  { }
};
