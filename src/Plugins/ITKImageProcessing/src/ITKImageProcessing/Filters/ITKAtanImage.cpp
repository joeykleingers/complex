#include "ITKAtanImage.hpp"

#include "ITKImageProcessing/Common/ITKArrayHelper.hpp"
#include "ITKImageProcessing/Common/sitkCommon.hpp"

#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/DataObjectNameParameter.hpp"
#include "complex/Parameters/GeometrySelectionParameter.hpp"

#include "complex/Utilities/SIMPLConversion.hpp"

#include <itkAtanImageFilter.h>

using namespace complex;

namespace cxITKAtanImage
{
using ArrayOptionsType = ITK::ScalarPixelIdTypeList;
// VectorPixelIDTypeList;

struct ITKAtanImageFunctor
{
  template <class InputImageT, class OutputImageT, uint32 Dimension>
  auto createFilter() const
  {
    using FilterType = itk::AtanImageFilter<InputImageT, OutputImageT>;
    auto filter = FilterType::New();
    return filter;
  }
};
} // namespace cxITKAtanImage

namespace complex
{
//------------------------------------------------------------------------------
std::string ITKAtanImage::name() const
{
  return FilterTraits<ITKAtanImage>::name;
}

//------------------------------------------------------------------------------
std::string ITKAtanImage::className() const
{
  return FilterTraits<ITKAtanImage>::className;
}

//------------------------------------------------------------------------------
Uuid ITKAtanImage::uuid() const
{
  return FilterTraits<ITKAtanImage>::uuid;
}

//------------------------------------------------------------------------------
std::string ITKAtanImage::humanName() const
{
  return "ITK Atan Image Filter";
}

//------------------------------------------------------------------------------
std::vector<std::string> ITKAtanImage::defaultTags() const
{
  return {className(), "ITKImageProcessing", "ITKAtanImage", "ITKImageIntensity", "ImageIntensity"};
}

//------------------------------------------------------------------------------
Parameters ITKAtanImage::parameters() const
{
  Parameters params;

  params.insertSeparator(Parameters::Separator{"Required Input Cell Data"});
  params.insert(std::make_unique<GeometrySelectionParameter>(k_SelectedImageGeomPath_Key, "Image Geometry", "Select the Image Geometry Group from the DataStructure.", DataPath({"Image Geometry"}),
                                                             GeometrySelectionParameter::AllowedTypes{IGeometry::Type::Image}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_SelectedImageDataPath_Key, "Input Image Data Array", "The image data that will be processed by this filter.", DataPath{},
                                                          complex::ITK::GetScalarPixelAllowedTypes()));

  params.insertSeparator(Parameters::Separator{"Created Cell Data"});
  params.insert(
      std::make_unique<DataObjectNameParameter>(k_OutputImageDataPath_Key, "Output Image Data Array", "The result of the processing will be stored in this Data Array.", "Output Image Data"));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer ITKAtanImage::clone() const
{
  return std::make_unique<ITKAtanImage>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult ITKAtanImage::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler, const std::atomic_bool& shouldCancel) const
{
  auto imageGeomPath = filterArgs.value<DataPath>(k_SelectedImageGeomPath_Key);
  auto selectedInputArray = filterArgs.value<DataPath>(k_SelectedImageDataPath_Key);
  auto outputArrayName = filterArgs.value<DataObjectNameParameter::ValueType>(k_OutputImageDataPath_Key);
  const DataPath outputArrayPath = selectedInputArray.getParent().createChildPath(outputArrayName);

  Result<OutputActions> resultOutputActions = ITK::DataCheck<cxITKAtanImage::ArrayOptionsType>(dataStructure, selectedInputArray, imageGeomPath, outputArrayPath);

  return {std::move(resultOutputActions)};
}

//------------------------------------------------------------------------------
Result<> ITKAtanImage::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                   const std::atomic_bool& shouldCancel) const
{
  auto imageGeomPath = filterArgs.value<DataPath>(k_SelectedImageGeomPath_Key);
  auto selectedInputArray = filterArgs.value<DataPath>(k_SelectedImageDataPath_Key);
  auto outputArrayName = filterArgs.value<DataObjectNameParameter::ValueType>(k_OutputImageDataPath_Key);
  const DataPath outputArrayPath = selectedInputArray.getParent().createChildPath(outputArrayName);

  const cxITKAtanImage::ITKAtanImageFunctor itkFunctor = {};

  auto& imageGeom = dataStructure.getDataRefAs<ImageGeom>(imageGeomPath);
  imageGeom.getLinkedGeometryData().addCellData(outputArrayPath);

  return ITK::Execute<cxITKAtanImage::ArrayOptionsType>(dataStructure, selectedInputArray, imageGeomPath, outputArrayPath, itkFunctor, shouldCancel);
}

namespace
{
namespace SIMPL
{
constexpr StringLiteral k_SelectedCellArrayPathKey = "SelectedCellArrayPath";
constexpr StringLiteral k_NewCellArrayNameKey = "NewCellArrayName";
} // namespace SIMPL
} // namespace

Result<Arguments> ITKAtanImage::FromSIMPLJson(const nlohmann::json& json)
{
  Arguments args = ITKAtanImage().getDefaultArguments();

  std::vector<Result<>> results;

  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::DataContainerSelectionFilterParameterConverter>(args, json, SIMPL::k_SelectedCellArrayPathKey, k_SelectedImageGeomPath_Key));
  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::DataArraySelectionFilterParameterConverter>(args, json, SIMPL::k_SelectedCellArrayPathKey, k_SelectedImageDataPath_Key));
  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::StringFilterParameterConverter>(args, json, SIMPL::k_NewCellArrayNameKey, k_OutputImageDataPath_Key));

  Result<> conversionResult = MergeResults(std::move(results));

  return ConvertResultTo<Arguments>(std::move(conversionResult), std::move(args));
}
} // namespace complex
