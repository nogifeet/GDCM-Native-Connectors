#include "gdcm_connector.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <gdcmAttribute.h>
#include <gdcmByteValue.h>
#include <gdcmDataElement.h>
#include <gdcmDataSet.h>
#include <gdcmFile.h>
#include <gdcmImage.h>
#include <gdcmImageReader.h>
#include <gdcmReader.h>
#include <gdcmStringFilter.h>
#include <gdcmTag.h>
#include <gdcmTransferSyntax.h>

namespace gdcm_connector {
namespace {

void read_file(const std::string& file_path, gdcm::Reader& reader) {
  reader.SetFileName(file_path.c_str());
  if (!reader.Read()) {
    throw std::runtime_error("Unable to read DICOM file: " + file_path);
  }
}

void read_image_file(const std::string& file_path, gdcm::ImageReader& reader) {
  reader.SetFileName(file_path.c_str());
  if (!reader.Read()) {
    throw std::runtime_error("Unable to read DICOM image pixel data: " + file_path);
  }
}

std::string trim(std::string value) {
  auto is_not_space = [](unsigned char ch) { return !std::isspace(ch) && ch != '\0'; };
  value.erase(value.begin(), std::find_if(value.begin(), value.end(), is_not_space));
  value.erase(std::find_if(value.rbegin(), value.rend(), is_not_space).base(), value.end());
  return value;
}

std::string tag_to_string(const gdcm::Tag& tag) {
  std::ostringstream stream;
  stream << tag;
  return stream.str();
}

std::string read_tag_from_reader(const gdcm::Reader& reader, const gdcm::Tag& tag) {
  gdcm::StringFilter filter;
  filter.SetFile(reader.GetFile());
  const std::string value = filter.ToString(tag);
  return trim(value);
}

std::string required_tag_from_reader(const gdcm::Reader& reader, const gdcm::Tag& tag) {
  const gdcm::DataSet& data_set = reader.GetFile().GetDataSet();
  if (!data_set.FindDataElement(tag)) {
    return "";
  }
  return read_tag_from_reader(reader, tag);
}

}  // namespace

std::string read_tag(const std::string& file_path, std::uint16_t group, std::uint16_t element) {
  gdcm::Reader reader;
  read_file(file_path, reader);
  const gdcm::Tag tag(group, element);
  const gdcm::DataSet& data_set = reader.GetFile().GetDataSet();
  if (!data_set.FindDataElement(tag)) {
    throw std::runtime_error("DICOM tag not found: " + tag_to_string(tag));
  }
  return read_tag_from_reader(reader, tag);
}

BasicMetadata read_basic_metadata(const std::string& file_path) {
  gdcm::Reader reader;
  read_file(file_path, reader);
  const gdcm::File& file = reader.GetFile();

  BasicMetadata metadata;
  metadata.patient_name = required_tag_from_reader(reader, gdcm::Tag(0x0010, 0x0010));
  metadata.patient_id = required_tag_from_reader(reader, gdcm::Tag(0x0010, 0x0020));
  metadata.study_instance_uid = required_tag_from_reader(reader, gdcm::Tag(0x0020, 0x000d));
  metadata.series_instance_uid = required_tag_from_reader(reader, gdcm::Tag(0x0020, 0x000e));
  metadata.sop_instance_uid = required_tag_from_reader(reader, gdcm::Tag(0x0008, 0x0018));
  metadata.modality = required_tag_from_reader(reader, gdcm::Tag(0x0008, 0x0060));
  metadata.transfer_syntax = file.GetHeader().GetDataSetTransferSyntax().GetString();
  return metadata;
}

PixelData read_pixels(const std::string& file_path) {
  gdcm::ImageReader reader;
  read_image_file(file_path, reader);

  const gdcm::Image& image = reader.GetImage();
  const gdcm::PixelFormat& pixel_format = image.GetPixelFormat();

  const unsigned long buffer_length = image.GetBufferLength();
  PixelData pixels;
  pixels.data.resize(static_cast<std::size_t>(buffer_length));
  if (buffer_length > 0 && !image.GetBuffer(reinterpret_cast<char*>(pixels.data.data()))) {
    throw std::runtime_error("Unable to extract DICOM pixel buffer: " + file_path);
  }

  const unsigned int number_of_dimensions = image.GetNumberOfDimensions();
  const unsigned int* dimensions = image.GetDimensions();
  pixels.dimensions.assign(dimensions, dimensions + number_of_dimensions);
  pixels.samples_per_pixel = pixel_format.GetSamplesPerPixel();
  pixels.bits_allocated = pixel_format.GetBitsAllocated();
  pixels.bits_stored = pixel_format.GetBitsStored();
  pixels.high_bit = pixel_format.GetHighBit();
  pixels.pixel_representation = pixel_format.GetPixelRepresentation();
  pixels.scalar_type = pixel_format.GetScalarTypeAsString();
  pixels.photometric_interpretation = image.GetPhotometricInterpretation().GetString();
  pixels.transfer_syntax = image.GetTransferSyntax().GetString();
  return pixels;
}

}  // namespace gdcm_connector
