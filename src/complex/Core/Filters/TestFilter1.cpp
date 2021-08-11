#include "TestFilter1.hpp"

#include "complex/Core/Parameters/BoolParameter.hpp"
#include "complex/Core/Parameters/NumberParameter.hpp"

using namespace complex;

namespace
{
constexpr const char k_Param1[] = "param1";
constexpr const char k_Param2[] = "param2";
} // namespace

namespace complex
{
std::string TestFilter1::name() const
{
  return "TestFilter1";
}

Uuid TestFilter1::uuid() const
{
  constexpr Uuid uuid = *Uuid::FromString("dd92896b-26ec-4419-b905-567e93e8f39d");
  return uuid;
}

std::string TestFilter1::humanName() const
{
  return "Test Filter 1";
}

Parameters TestFilter1::parameters() const
{
  Parameters params;
  params.insert(std::make_unique<Float32Parameter>(k_Param1, "Parameter 1", "The 1st parameter", 0.0f));
  params.insert(std::make_unique<BoolParameter>(k_Param2, "Parameter 2", "The 2nd parameter", false));
  return params;
}

Result<OutputActions> TestFilter1::preflightImpl(const DataStructure& data, const Arguments& args, const MessageHandler& messageHandler) const
{
  return {};
}

Result<> TestFilter1::executeImpl(DataStructure& data, const Arguments& args, const MessageHandler& messageHandler) const
{
  return {};
}
} // namespace complex
