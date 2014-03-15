#pragma once
#include <json-cpp/forwards.h>
#include "mxprops.h"



namespace mxprops {

MXPROPS_API
bool load_from_command_line(mxprops::PTree::Ref const& dst,
                            std::vector<std::string> & messages, 
                            int argc, 
                            char const* argv[]);

MXPROPS_API
bool load_from_json(mxprops::PTree::Ref const& dst,
                    std::vector<std::string> & messages, 
                    Json::Value const& doc);

MXPROPS_API
bool load_from_json_file(mxprops::PTree::Ref const& dst,
                         std::vector<std::string> & messages, 
                         std::string const& filename);

}
