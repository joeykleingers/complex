#include "ReadVtkStructuredPoints.hpp"

#include "simplnx/DataStructure/DataArray.hpp"
#include "simplnx/DataStructure/DataGroup.hpp"

using namespace nx::core;

// -----------------------------------------------------------------------------
ReadVtkStructuredPoints::ReadVtkStructuredPoints(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel,
                                                 ReadVtkStructuredPointsInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(mesgHandler)
{
}

// -----------------------------------------------------------------------------
ReadVtkStructuredPoints::~ReadVtkStructuredPoints() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& ReadVtkStructuredPoints::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> ReadVtkStructuredPoints::operator()()
{
  /**
   * This section of the code should contain the actual algorithmic codes that
   * will accomplish the goal of the file.
   *
   * If you can parallelize the code there are a number of examples on how to do that.
   *    GenerateIPFColors is one example
   *
   * If you need to determine what kind of array you have (Int32Array, Float32Array, etc)
   * look to the ExecuteDataFunction() in simplnx/Utilities/FilterUtilities.hpp template
   * function to help with that code.
   *   An Example algorithm class is `CombineAttributeArrays` and `RemoveFlaggedVertices`
   *
   * There are other utility classes that can help alleviate the amount of code that needs
   * to be written.
   *
   * REMOVE THIS COMMENT BLOCK WHEN YOU ARE FINISHED WITH THE FILTER_HUMAN_NAME
   */

  return {};
}