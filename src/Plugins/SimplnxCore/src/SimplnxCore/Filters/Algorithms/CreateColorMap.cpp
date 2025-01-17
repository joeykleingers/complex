#include "CreateColorMap.hpp"

#include "simplnx/DataStructure/DataArray.hpp"
#include "simplnx/DataStructure/DataGroup.hpp"
#include "simplnx/Utilities/ColorTableUtilities.hpp"
#include "simplnx/Utilities/FilterUtilities.hpp"
#include "simplnx/Utilities/ParallelDataAlgorithm.hpp"

using namespace nx::core;

namespace
{
constexpr usize k_ControlPointCompSize = 4;

// -----------------------------------------------------------------------------
usize findRightBinIndex(float32 nValue, const std::vector<float32>& binPoints)
{
  usize min = 0;
  usize max = binPoints.size() - 1;
  while(min < max)
  {
    const usize middle = (min + max) / 2;
    if(nValue > binPoints[middle])
    {
      min = middle + 1;
    }
    else
    {
      max = middle;
    }
  }
  return min;
}

/**
 * @brief The CreateColorMapImpl class implements a threaded algorithm that computes the RGB values
 * for each element in a given array of data
 */
template <typename T>
class CreateColorMapImpl
{
public:
  CreateColorMapImpl(const AbstractDataStore<T>& arrayStore, const std::vector<float32>& binPoints, const std::vector<float32>& controlPoints, int numControlColors, UInt8AbstractDataStore& colorStore,
                     const nx::core::IDataArray* goodVoxels, const std::vector<uint8>& invalidColor)
  : m_ArrayStore(arrayStore)
  , m_BinPoints(binPoints)
  , m_NumControlColors(numControlColors)
  , m_ControlPoints(controlPoints)
  , m_ColorStore(colorStore)
  , m_ArrayMin(arrayStore[0])
  , m_ArrayMax(arrayStore[0])
  , m_GoodVoxels(goodVoxels)
  , m_InvalidColor(invalidColor)
  {
    for(usize i = 1; i < arrayStore.getNumberOfTuples(); i++)
    {
      if(arrayStore[i] < m_ArrayMin)
      {
        m_ArrayMin = arrayStore[i];
      }
      if(arrayStore[i] > m_ArrayMax)
      {
        m_ArrayMax = arrayStore[i];
      }
    }
  }

  template <typename K>
  void convert(usize start, usize end) const
  {
    using MaskArrayType = DataArray<K>;
    const MaskArrayType* maskArray = nullptr;
    if(nullptr != m_GoodVoxels)
    {
      maskArray = dynamic_cast<const MaskArrayType*>(m_GoodVoxels);
    }

    for(size_t i = start; i < end; i++)
    {
      // Make sure we are using a valid voxel based on the "goodVoxels" arrays
      if(nullptr != maskArray)
      {
        if(!(*maskArray)[i])
        {
          m_ColorStore.setComponent(i, 0, m_InvalidColor[0]);
          m_ColorStore.setComponent(i, 1, m_InvalidColor[1]);
          m_ColorStore.setComponent(i, 2, m_InvalidColor[2]);
          continue;
        }
      }

      // Normalize value
      const float32 nValue = (static_cast<float32>(m_ArrayStore[i] - m_ArrayMin)) / static_cast<float32>((m_ArrayMax - m_ArrayMin));

      int rightBinIndex = findRightBinIndex(nValue, m_BinPoints);

      int leftBinIndex = rightBinIndex - 1;
      if(leftBinIndex < 0)
      {
        leftBinIndex = 0;
        rightBinIndex = 1;
      }

      // Find the fractional distance traveled between the beginning and end of the current color bin
      float32 currFraction;
      if(rightBinIndex < m_BinPoints.size())
      {
        currFraction = (nValue - m_BinPoints[leftBinIndex]) / (m_BinPoints[rightBinIndex] - m_BinPoints[leftBinIndex]);
      }
      else
      {
        currFraction = (nValue - m_BinPoints[leftBinIndex]) / (1 - m_BinPoints[leftBinIndex]);
      }

      // If the current color bin index is larger than the total number of control colors, automatically set the currentBinIndex
      // to the last control color.
      if(leftBinIndex > m_NumControlColors - 1)
      {
        leftBinIndex = m_NumControlColors - 1;
      }

      // Calculate the RGB values
      const unsigned char redVal =
          (m_ControlPoints[leftBinIndex * k_ControlPointCompSize + 1] * (1.0 - currFraction) + m_ControlPoints[rightBinIndex * k_ControlPointCompSize + 1] * currFraction) * 255;
      const unsigned char greenVal =
          (m_ControlPoints[leftBinIndex * k_ControlPointCompSize + 2] * (1.0 - currFraction) + m_ControlPoints[rightBinIndex * k_ControlPointCompSize + 2] * currFraction) * 255;
      const unsigned char blueVal =
          (m_ControlPoints[leftBinIndex * k_ControlPointCompSize + 3] * (1.0 - currFraction) + m_ControlPoints[rightBinIndex * k_ControlPointCompSize + 3] * currFraction) * 255;

      m_ColorStore.setComponent(i, 0, redVal);
      m_ColorStore.setComponent(i, 1, greenVal);
      m_ColorStore.setComponent(i, 2, blueVal);
    }
  }

  void operator()(const Range& range) const
  {
    if(m_GoodVoxels != nullptr)
    {
      if(m_GoodVoxels->getDataType() == DataType::boolean)
      {
        convert<bool>(range.min(), range.max());
      }
      else if(m_GoodVoxels->getDataType() == DataType::uint8)
      {
        convert<uint8>(range.min(), range.max());
      }
    }
    else
    {
      convert<bool>(range.min(), range.max());
    }
  }

private:
  const AbstractDataStore<T>& m_ArrayStore;
  const std::vector<float32>& m_BinPoints;
  T m_ArrayMin;
  T m_ArrayMax;
  int m_NumControlColors;
  const std::vector<float32>& m_ControlPoints;
  UInt8AbstractDataStore& m_ColorStore;
  const nx::core::IDataArray* m_GoodVoxels = nullptr;
  const std::vector<uint8>& m_InvalidColor;
};

struct GenerateColorArrayFunctor
{
  template <typename ScalarType>
  Result<> operator()(DataStructure& dataStructure, const CreateColorMapInputValues* inputValues, const std::vector<float32>& controlPoints)
  {
    // Control Points is a flattened 2D array with an unknown tuple count and a component size of 4
    const usize numControlColors = controlPoints.size() / k_ControlPointCompSize;

    // Store A-values in binPoints vector.
    std::vector<float32> binPoints;
    binPoints.reserve(numControlColors);
    for(usize i = 0; i < numControlColors; i++)
    {
      binPoints.push_back(controlPoints[i * k_ControlPointCompSize]);
    }

    // Normalize binPoints values
    const float32 binMin = binPoints[0];
    const float32 binMax = binPoints[binPoints.size() - 1];
    for(auto& binPoint : binPoints)
    {
      binPoint = (binPoint - binMin) / (binMax - binMin);
    }

    auto& colorArray = dataStructure.getDataAs<UInt8Array>(inputValues->RgbArrayPath)->getDataStoreRef();

    nx::core::IDataArray* goodVoxelsArray = nullptr;
    if(inputValues->UseMask)
    {
      goodVoxelsArray = dataStructure.getDataAs<IDataArray>(inputValues->MaskArrayPath);
    }

    const AbstractDataStore<ScalarType>& arrayRef = dataStructure.getDataAs<DataArray<ScalarType>>(inputValues->SelectedDataArrayPath)->getDataStoreRef();
    if(arrayRef.getNumberOfTuples() <= 0)
    {
      return MakeErrorResult(-34381, fmt::format("Array {} is empty", inputValues->SelectedDataArrayPath.getTargetName()));
    }

    ParallelDataAlgorithm dataAlg;
    dataAlg.setRange(0, arrayRef.getNumberOfTuples());
    dataAlg.execute(CreateColorMapImpl(arrayRef, binPoints, controlPoints, numControlColors, colorArray, goodVoxelsArray, inputValues->InvalidColor));
    return {};
  }
};
} // namespace

// -----------------------------------------------------------------------------
CreateColorMap::CreateColorMap(DataStructure& dataStructure, const IFilter::MessageHandler& msgHandler, const std::atomic_bool& shouldCancel, CreateColorMapInputValues* inputValues)
: m_DataStructure(dataStructure)
, m_InputValues(inputValues)
, m_ShouldCancel(shouldCancel)
, m_MessageHandler(msgHandler)
{
}

// -----------------------------------------------------------------------------
CreateColorMap::~CreateColorMap() noexcept = default;

// -----------------------------------------------------------------------------
const std::atomic_bool& CreateColorMap::getCancel()
{
  return m_ShouldCancel;
}

// -----------------------------------------------------------------------------
Result<> CreateColorMap::operator()()
{
  const IDataArray& selectedIDataArray = m_DataStructure.getDataRefAs<IDataArray>(m_InputValues->SelectedDataArrayPath);

  auto controlPointsResult = ColorTableUtilities::ExtractControlPoints(m_InputValues->PresetName);
  if(controlPointsResult.invalid())
  {
    auto error = *controlPointsResult.errors().begin();
    return MakeErrorResult(error.code, error.message);
  }
  auto controlPoints = controlPointsResult.value();
  if(controlPoints.empty())
  {
    return MakeErrorResult(-34380, fmt::format("No valid points found from preset {}", m_InputValues->PresetName));
  }

  ExecuteDataFunction(GenerateColorArrayFunctor{}, selectedIDataArray.getDataType(), m_DataStructure, m_InputValues, controlPoints);
  return {};
}
