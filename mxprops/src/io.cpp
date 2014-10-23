#define MXPROPS_EXPORTS
#include "../io.h"
#include <json-cpp/value.h>
#include <json-cpp/reader.h>
#include <sstream>
#include <fstream>
#include <boost/algorithm/string/trim.hpp>


namespace mxprops {

static void add_prop_line(mxprops::PTree::Ref const& dst,
                          std::string const& line)
{
  std::string trimmed = line;
  boost::trim(trimmed);
  if (line.empty() || line[0] == '#')
    return;

  size_t const eqPos = trimmed.find('=');
  if (eqPos == std::string::npos)
    throw std::runtime_error("Bad syntax: cannot find '=' in string: " + trimmed);
  std::string namePart = trimmed.substr(0, eqPos);
  boost::trim_right(namePart);
  std::string valuePart = trimmed.substr(eqPos + 1);
  boost::trim_right(valuePart);

  dst.set(namePart, valuePart);
}

bool load_from_command_line(mxprops::PTree::Ref const& dst,
                            std::vector<std::string> & messages,
                            int argc,
                            char const* argv[])
{
  try
  {
    for (int i = 1; i < argc; ++i)
    {
      if (argv[i][0] == '-')
      {
        add_prop_line(dst, argv[i] + 1);
      }
      else
      {
        std::ostringstream oss;
        oss << "unknown arg #" << i << ": '" << argv[i] << "'";
        throw std::runtime_error(oss.str());
      }
    }
    return true;
  }
  catch (std::runtime_error const& e)
  {
    messages.push_back(e.what());
    return false;
  }
}

bool load_from_json(mxprops::PTree::Ref const& dst,
                    std::vector<std::string> & messages,
                    Json::Value const& doc)
{
  std::vector<std::string> members = doc.getMemberNames();
  for (size_t i = 0; i < members.size(); ++i)
  {
    std::string const& n = members[i];
    Json::Value const& v = doc[n];
    Json::ValueType const ty = v.type();

    switch (ty)
    {
    case Json::objectValue:
      if (!load_from_json(dst.getSubtree(n), messages, v))
        return false;
      break;

    case Json::arrayValue:
      {
        Json::Value objv(Json::objectValue);
        for (size_t ai = 0; ai < v.size(); ++ai)
          objv[boost::lexical_cast<std::string>(ai)] = v[ai];
        return load_from_json(dst, messages, objv);
      }

    case Json::nullValue:
      dst.undefine(n);
      break;

    case Json::intValue:
      dst.set(n, v.asInt());
      break;
    case Json::uintValue:
      dst.set(n, v.asUInt());
      break;
    case Json::realValue:
      dst.set(n, v.asDouble());
      break;
    case Json::stringValue:
      dst.set(n, v.asString());
      break;
    case Json::booleanValue:
      dst.set<int>(n, v.asBool());
      break;

    default:
      messages.push_back("unknown value type");
      return false;
    }
  }
  return true;
}


bool load_from_json_file(mxprops::PTree::Ref const& dst,
                         std::vector<std::string> & messages,
                         std::string const& filename)
{
  Json::Reader reader;
  Json::Value doc(Json::objectValue);
  std::ifstream f(filename.c_str());
  if (!f.is_open() || !reader.parse(f, doc))
  {
    messages.push_back("Failed to parse json file " + filename + ": "
                       + reader.getFormatedErrorMessages());
    return false;
  }
  return load_from_json(dst, messages, doc);
}

void init_settings_from_command_line(mxprops::PTree::Ref const& dst,
                                     int argc,
                                     char const* argv[])
{
  std::vector<std::string> messages;

  for (int i = 1; i < argc; ++i)
  {
    if (argv[i][0] == '-')
    {
      add_prop_line(dst, argv[i] + 1);
    }
    else if (!load_from_json_file(dst, messages, argv[i]))
    {
      std::ostringstream oss;
      oss << "unknown arg #" << i << ": '" << argv[i] << "'";
      throw std::runtime_error(oss.str());
    }
  }
}

}
