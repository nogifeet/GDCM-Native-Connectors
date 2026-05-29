import sys
sys.path.insert(0, "../../build/python")

import gdcm_connector
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image
from IPython.display import display, Markdown

sample_file_path = "../../dicom_samples/1-02.dcm"

metadata = gdcm_connector.read_basic_metadata(sample_file_path)

print(f"Metadata:\n {metadata}")

modality = gdcm_connector.read_tag(sample_file_path, 0x0008, 0x0060)

print(f"Modality:\n {modality}")

pixels = gdcm_connector.read_pixels(sample_file_path)

print("Pixel Keys:", end="\n")
print(pixels.keys())

dtype_map = {
    "UINT8": np.uint8,
    "INT8": np.int8,
    "UINT16": np.uint16,
    "INT16": np.int16,
    "UINT32": np.uint32,
    "INT32": np.int32,
    "FLOAT32": np.float32,
    "FLOAT64": np.float64,
}

dtype = dtype_map[pixels["scalar_type"]]
arr = np.frombuffer(pixels["data"], dtype=dtype)

width, height = pixels["dimensions"][:2]
samples = pixels["samples_per_pixel"]

if samples == 1:
    image = arr.reshape(height, width)
else:
    image = arr.reshape(height, width, samples)

image_path = "./dicom_image.jpg"
Image.fromarray(image).save(image_path, quality=90)