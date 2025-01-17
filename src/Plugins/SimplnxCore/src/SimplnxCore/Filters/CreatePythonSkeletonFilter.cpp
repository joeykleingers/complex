#include "CreatePythonSkeletonFilter.hpp"

#include "SimplnxCore/Filters/Algorithms/CreatePythonSkeleton.hpp"

#include "simplnx/DataStructure/DataPath.hpp"
#include "simplnx/Filter/Actions/CreateArrayAction.hpp"
#include "simplnx/Parameters/BoolParameter.hpp"
#include "simplnx/Parameters/StringParameter.hpp"
#include "simplnx/Utilities/SIMPLConversion.hpp"
#include "simplnx/Utilities/StringUtilities.hpp"

#include <filesystem>
namespace fs = std::filesystem;

using namespace nx::core;

namespace nx::core
{
//------------------------------------------------------------------------------
std::string CreatePythonSkeletonFilter::name() const
{
  return FilterTraits<CreatePythonSkeletonFilter>::name.str();
}

//------------------------------------------------------------------------------
std::string CreatePythonSkeletonFilter::className() const
{
  return FilterTraits<CreatePythonSkeletonFilter>::className;
}

//------------------------------------------------------------------------------
Uuid CreatePythonSkeletonFilter::uuid() const
{
  return FilterTraits<CreatePythonSkeletonFilter>::uuid;
}

//------------------------------------------------------------------------------
std::string CreatePythonSkeletonFilter::humanName() const
{
  return "Create Python Plugin and/or Filters";
}

//------------------------------------------------------------------------------
std::vector<std::string> CreatePythonSkeletonFilter::defaultTags() const
{
  return {className(), "Generic", "Python",    "Plugin", "Skeleton", "Generate", "Create", "Template", "Code",  "Produce",
          "Form",      "Develop", "Construct", "Make",   "Build",    "Engineer", "Invent", "Initiate", "Design"};
}

//------------------------------------------------------------------------------
Parameters CreatePythonSkeletonFilter::parameters() const
{
  Parameters params;

  params.insertLinkableParameter(
      std::make_unique<BoolParameter>(k_UseExistingPlugin_Key, "Use Existing Plugin", "Generate the list of filters into an existing plugin instead of creating a new plugin.", false));
  params.insert(std::make_unique<StringParameter>(k_PluginName_Key, "Name of Plugin", "This is the name of the plugin.", "ExamplePlugin"));
  params.insert(std::make_unique<StringParameter>(k_PluginHumanName_Key, "Human Name of Plugin", "This is the user facing name of the plugin.", "ExamplePlugin"));

  params.insert(std::make_unique<FileSystemPathParameter>(k_PluginOutputDirectory_Key, "Plugin Output Directory", "The path to the output directory where the new plugin will be generated.",
                                                          fs::path(""), FileSystemPathParameter::ExtensionsType{}, FileSystemPathParameter::PathType::OutputDir));
  params.insert(std::make_unique<FileSystemPathParameter>(k_PluginInputDirectory_Key, "Existing Plugin Location", "The location of the existing plugin's top level directory on the file system.",
                                                          fs::path(""), FileSystemPathParameter::ExtensionsType{}, FileSystemPathParameter::PathType::InputDir));

  params.insert(
      std::make_unique<StringParameter>(k_PluginFilterNames, "Filter Names (comma-separated)", "The names of filters that will be created, separated by commas (,).", "FirstFilter,SecondFilter"));

  params.linkParameters(k_UseExistingPlugin_Key, k_PluginName_Key, false);
  params.linkParameters(k_UseExistingPlugin_Key, k_PluginHumanName_Key, false);
  params.linkParameters(k_UseExistingPlugin_Key, k_PluginOutputDirectory_Key, false);
  params.linkParameters(k_UseExistingPlugin_Key, k_PluginInputDirectory_Key, true);
  return params;
}

//------------------------------------------------------------------------------
IFilter::VersionType CreatePythonSkeletonFilter::parametersVersion() const
{
  return 1;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer CreatePythonSkeletonFilter::clone() const
{
  return std::make_unique<CreatePythonSkeletonFilter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult CreatePythonSkeletonFilter::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                                   const std::atomic_bool& shouldCancel) const
{
  auto useExistingPlugin = filterArgs.value<BoolParameter::ValueType>(k_UseExistingPlugin_Key);
  auto pluginOutputDir = filterArgs.value<FileSystemPathParameter::ValueType>(k_PluginOutputDirectory_Key);
  auto pluginName = filterArgs.value<StringParameter::ValueType>(k_PluginName_Key);
  auto pluginInputDir = filterArgs.value<FileSystemPathParameter::ValueType>(k_PluginInputDirectory_Key);
  auto filterNames = filterArgs.value<StringParameter::ValueType>(k_PluginFilterNames);

  nx::core::Result<OutputActions> resultOutputActions;
  std::vector<PreflightValue> preflightUpdatedValues;

  auto filterList = StringUtilities::split(filterNames, ',');

  std::stringstream preflightUpdatedValue;

  std::string pluginPath = fmt::format("{}{}{}", pluginOutputDir.string(), std::string{fs::path::preferred_separator}, pluginName);
  if(useExistingPlugin)
  {
    pluginPath = pluginInputDir.string();
    pluginName = pluginInputDir.filename().string();
  }

  std::string fullPath = fmt::format("{}{}{}{}meta.yaml", pluginPath, std::string{fs::path::preferred_separator}, "conda", std::string{fs::path::preferred_separator});
  if(std::filesystem::exists({fullPath}))
  {
    fullPath = std::string("[REPLACE]: ").append(fullPath);
  }
  else
  {
    fullPath = std::string("[New]: ").append(fullPath);
  }
  preflightUpdatedValue << fullPath << '\n';

  fullPath = fmt::format("{}{}{}{}environment.yml", pluginOutputDir.string(), std::string{fs::path::preferred_separator}, pluginName, std::string{fs::path::preferred_separator});
  if(std::filesystem::exists({fullPath}))
  {
    fullPath = std::string("[REPLACE]: ").append(fullPath);
  }
  else
  {
    fullPath = std::string("[New]: ").append(fullPath);
  }
  preflightUpdatedValue << fullPath << '\n';

  fullPath = fmt::format("{}{}{}{}pyproject.toml", pluginOutputDir.string(), std::string{fs::path::preferred_separator}, pluginName, std::string{fs::path::preferred_separator});
  if(std::filesystem::exists({fullPath}))
  {
    fullPath = std::string("[REPLACE]: ").append(fullPath);
  }
  else
  {
    fullPath = std::string("[New]: ").append(fullPath);
  }
  preflightUpdatedValue << fullPath << '\n';

  fullPath = fmt::format("{}{}{}{}{}{}__init__.py", pluginPath, std::string{fs::path::preferred_separator}, "src", std::string{fs::path::preferred_separator}, pluginName,
                         std::string{fs::path::preferred_separator});
  if(std::filesystem::exists({fullPath}))
  {
    fullPath = std::string("[REPLACE]: ").append(fullPath);
  }
  else
  {
    fullPath = std::string("[New]: ").append(fullPath);
  }
  preflightUpdatedValue << fullPath << '\n';

  fullPath = fmt::format("{}{}{}{}{}{}Plugin.py", pluginPath, std::string{fs::path::preferred_separator}, "src", std::string{fs::path::preferred_separator}, pluginName,
                         std::string{fs::path::preferred_separator});
  if(std::filesystem::exists({fullPath}))
  {
    fullPath = std::string("[REPLACE]: ").append(fullPath);
  }
  else
  {
    fullPath = std::string("[New]: ").append(fullPath);
  }
  preflightUpdatedValue << fullPath << '\n';

  for(const auto& filterName : filterList)
  {
    fullPath = fmt::format("{}{}{}{}{}{}{}.py", pluginPath, std::string{fs::path::preferred_separator}, "src", std::string{fs::path::preferred_separator}, pluginName,
                           std::string{fs::path::preferred_separator}, filterName);
    if(std::filesystem::exists({fullPath}))
    {
      fullPath = std::string("[REPLACE]: ").append(fullPath);
    }
    else
    {
      fullPath = std::string("[New]: ").append(fullPath);
    }
    preflightUpdatedValue << fullPath << '\n';
  }

  preflightUpdatedValues.push_back({"Generated Plugin File(s):", preflightUpdatedValue.str()});
  preflightUpdatedValues.push_back({"Warning:", "Any Existing Files Will Be Overwritten"});

  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> CreatePythonSkeletonFilter::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                                 const std::atomic_bool& shouldCancel) const
{
  CreatePythonSkeletonInputValues inputValues;

  inputValues.useExistingPlugin = filterArgs.value<BoolParameter::ValueType>(k_UseExistingPlugin_Key);
  inputValues.pluginInputDir = filterArgs.value<FileSystemPathParameter::ValueType>(k_PluginInputDirectory_Key);
  inputValues.pluginOutputDir = filterArgs.value<FileSystemPathParameter::ValueType>(k_PluginOutputDirectory_Key);
  inputValues.pluginName = filterArgs.value<StringParameter::ValueType>(k_PluginName_Key);
  inputValues.pluginHumanName = filterArgs.value<StringParameter::ValueType>(k_PluginHumanName_Key);
  inputValues.filterNames = filterArgs.value<StringParameter::ValueType>(k_PluginFilterNames);

  return CreatePythonSkeleton(dataStructure, messageHandler, shouldCancel, &inputValues)();
}

} // namespace nx::core
