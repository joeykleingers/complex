#pragma once

#include "ITKImageProcessing/ITKImageProcessing_export.hpp"

#include "simplnx/Filter/FilterTraits.hpp"
#include "simplnx/Filter/IFilter.hpp"

namespace nx::core
{
/**
 * @class ITKMedianProjectionImageFilter
 * @brief Median projection.
 *
 * This class was contributed to the Insight Journal by Gaetan Lehmann. The original paper can be found at https://www.insight-journal.org/browse/publication/71
 *
 * @author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 *
 * @see ProjectionImageFilter
 *
 *
 * @see MeanProjectionImageFilter
 *
 *
 * @see MinimumProjectionImageFilter
 *
 *
 * @see StandardDeviationProjectionImageFilter
 *
 *
 * @see SumProjectionImageFilter
 *
 *
 * @see BinaryProjectionImageFilter
 *
 *
 * @see MaximumProjectionImageFilter
 *
 * ITK Module: ITKImageStatistics
 * ITK Group: ImageStatistics
 */
class ITKIMAGEPROCESSING_EXPORT ITKMedianProjectionImageFilter : public IFilter
{
public:
  ITKMedianProjectionImageFilter() = default;
  ~ITKMedianProjectionImageFilter() noexcept override = default;

  ITKMedianProjectionImageFilter(const ITKMedianProjectionImageFilter&) = delete;
  ITKMedianProjectionImageFilter(ITKMedianProjectionImageFilter&&) noexcept = delete;

  ITKMedianProjectionImageFilter& operator=(const ITKMedianProjectionImageFilter&) = delete;
  ITKMedianProjectionImageFilter& operator=(ITKMedianProjectionImageFilter&&) noexcept = delete;

  // Parameter Keys
  static inline constexpr StringLiteral k_ProjectionDimension_Key = "projection_dimension";
  static inline constexpr StringLiteral k_RemoveOriginalGeometry_Key = "remove_original_geometry";
  static inline constexpr StringLiteral k_InputImageGeomPath_Key = "input_image_geometry_path";
  static inline constexpr StringLiteral k_InputImageDataPath_Key = "input_image_data_path";
  static inline constexpr StringLiteral k_OutputImageGeomName_Key = "output_image_geometry_name";
  static inline constexpr StringLiteral k_OutputImageArrayName_Key = "output_array_name";

  /**
   * @brief Returns the name of the filter.
   * @return
   */
  std::string name() const override;

  /**
   * @brief Returns the C++ classname of this filter.
   * @return
   */
  std::string className() const override;

  /**
   * @brief Returns the uuid of the filter.
   * @return
   */
  Uuid uuid() const override;

  /**
   * @brief Returns the human readable name of the filter.
   * @return
   */
  std::string humanName() const override;

  /**
   * @brief Returns the default tags for this filter.
   * @return
   */
  std::vector<std::string> defaultTags() const override;

  /**
   * @brief Returns the parameters of the filter (i.e. its inputs)
   * @return
   */
  Parameters parameters() const override;

  /**
   * @brief Returns parameters version integer.
   * Initial version should always be 1.
   * Should be incremented everytime the parameters change.
   * @return VersionType
   */
  VersionType parametersVersion() const override;

  /**
   * @brief Returns a copy of the filter.
   * @return
   */
  UniquePointer clone() const override;

protected:
  /**
   * @brief Takes in a DataStructure and checks that the filter can be run on it with the given arguments.
   * Returns any warnings/errors. Also returns the changes that would be applied to the DataStructure.
   * Some parts of the actions may not be completely filled out if all the required information is not available at preflight time.
   * @param dataStructure The input DataStructure instance
   * @param filterArgs These are the input values for each parameter that is required for the filter
   * @param messageHandler The MessageHandler object
   * @param shouldCancel Boolean that gets set if the filter should stop executing and return
   * @return Returns a Result object with error or warning values if any of those occurred during execution of this function
   */
  PreflightResult preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler, const std::atomic_bool& shouldCancel) const override;

  /**
   * @brief Applies the filter's algorithm to the DataStructure with the given arguments. Returns any warnings/errors.
   * On failure, there is no guarantee that the DataStructure is in a correct state.
   * @param dataStructure The input DataStructure instance
   * @param filterArgs These are the input values for each parameter that is required for the filter
   * @param messageHandler The MessageHandler object
   * @param shouldCancel Boolean that gets set if the filter should stop executing and return
   * @return Returns a Result object with error or warning values if any of those occurred during execution of this function
   */
  Result<> executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                       const std::atomic_bool& shouldCancel) const override;
};
} // namespace nx::core

SIMPLNX_DEF_FILTER_TRAITS(nx::core, ITKMedianProjectionImageFilter, "00e48f6b-8a00-414f-b3d9-49d48a3f9a00");