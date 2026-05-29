#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace gdcm_connector {

struct BasicMetadata {
  std::string patient_name;
  std::string patient_id;
  std::string study_instance_uid;
  std::string series_instance_uid;
  std::string sop_instance_uid;
  std::string modality;
  std::string transfer_syntax;
};

struct PixelData {
  std::vector<unsigned char> data;
  std::vector<unsigned int> dimensions;
  unsigned short samples_per_pixel;
  unsigned short bits_allocated;
  unsigned short bits_stored;
  unsigned short high_bit;
  unsigned short pixel_representation;
  std::string scalar_type;
  std::string photometric_interpretation;
  std::string transfer_syntax;
};

std::string read_tag(const std::string& file_path, std::uint16_t group, std::uint16_t element);
BasicMetadata read_basic_metadata(const std::string& file_path);
PixelData read_pixels(const std::string& file_path);

}  // namespace gdcm_connector
