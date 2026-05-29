#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <cstdint>
#include <exception>
#include <string>

#include "gdcm_connector.hpp"

namespace {

PyObject* to_python_error(const std::exception& error) {
  PyErr_SetString(PyExc_RuntimeError, error.what());
  return nullptr;
}

PyObject* py_read_tag(PyObject*, PyObject* args) {
  const char* file_path = nullptr;
  unsigned int group = 0;
  unsigned int element = 0;

  if (!PyArg_ParseTuple(args, "sII", &file_path, &group, &element)) {
    return nullptr;
  }

  try {
    const std::string value = gdcm_connector::read_tag(
      file_path,
      static_cast<std::uint16_t>(group),
      static_cast<std::uint16_t>(element));
    return PyUnicode_FromStringAndSize(value.c_str(), static_cast<Py_ssize_t>(value.size()));
  } catch (const std::exception& error) {
    return to_python_error(error);
  }
}

int set_string(PyObject* dict, const char* key, const std::string& value) {
  PyObject* py_value = PyUnicode_FromStringAndSize(value.c_str(), static_cast<Py_ssize_t>(value.size()));
  if (!py_value) {
    return -1;
  }
  const int result = PyDict_SetItemString(dict, key, py_value);
  Py_DECREF(py_value);
  return result;
}

int set_unsigned_long(PyObject* dict, const char* key, unsigned long value) {
  PyObject* py_value = PyLong_FromUnsignedLong(value);
  if (!py_value) {
    return -1;
  }
  const int result = PyDict_SetItemString(dict, key, py_value);
  Py_DECREF(py_value);
  return result;
}

int set_dimensions(PyObject* dict, const std::vector<unsigned int>& dimensions) {
  PyObject* tuple = PyTuple_New(static_cast<Py_ssize_t>(dimensions.size()));
  if (!tuple) {
    return -1;
  }

  for (std::size_t i = 0; i < dimensions.size(); ++i) {
    PyObject* value = PyLong_FromUnsignedLong(dimensions[i]);
    if (!value) {
      Py_DECREF(tuple);
      return -1;
    }
    PyTuple_SET_ITEM(tuple, static_cast<Py_ssize_t>(i), value);
  }

  const int result = PyDict_SetItemString(dict, "dimensions", tuple);
  Py_DECREF(tuple);
  return result;
}

PyObject* py_read_basic_metadata(PyObject*, PyObject* args) {
  const char* file_path = nullptr;
  if (!PyArg_ParseTuple(args, "s", &file_path)) {
    return nullptr;
  }

  try {
    const gdcm_connector::BasicMetadata metadata = gdcm_connector::read_basic_metadata(file_path);
    PyObject* dict = PyDict_New();
    if (!dict) {
      return nullptr;
    }

    if (set_string(dict, "patient_name", metadata.patient_name) < 0 ||
        set_string(dict, "patient_id", metadata.patient_id) < 0 ||
        set_string(dict, "study_instance_uid", metadata.study_instance_uid) < 0 ||
        set_string(dict, "series_instance_uid", metadata.series_instance_uid) < 0 ||
        set_string(dict, "sop_instance_uid", metadata.sop_instance_uid) < 0 ||
        set_string(dict, "modality", metadata.modality) < 0 ||
        set_string(dict, "transfer_syntax", metadata.transfer_syntax) < 0) {
      Py_DECREF(dict);
      return nullptr;
    }

    return dict;
  } catch (const std::exception& error) {
    return to_python_error(error);
  }
}

PyObject* py_read_pixels(PyObject*, PyObject* args) {
  const char* file_path = nullptr;
  if (!PyArg_ParseTuple(args, "s", &file_path)) {
    return nullptr;
  }

  try {
    const gdcm_connector::PixelData pixels = gdcm_connector::read_pixels(file_path);
    PyObject* dict = PyDict_New();
    if (!dict) {
      return nullptr;
    }

    PyObject* data = PyBytes_FromStringAndSize(
      reinterpret_cast<const char*>(pixels.data.data()),
      static_cast<Py_ssize_t>(pixels.data.size()));
    if (!data) {
      Py_DECREF(dict);
      return nullptr;
    }

    const int data_result = PyDict_SetItemString(dict, "data", data);
    Py_DECREF(data);

    if (data_result < 0 ||
        set_dimensions(dict, pixels.dimensions) < 0 ||
        set_unsigned_long(dict, "samples_per_pixel", pixels.samples_per_pixel) < 0 ||
        set_unsigned_long(dict, "bits_allocated", pixels.bits_allocated) < 0 ||
        set_unsigned_long(dict, "bits_stored", pixels.bits_stored) < 0 ||
        set_unsigned_long(dict, "high_bit", pixels.high_bit) < 0 ||
        set_unsigned_long(dict, "pixel_representation", pixels.pixel_representation) < 0 ||
        set_string(dict, "scalar_type", pixels.scalar_type) < 0 ||
        set_string(dict, "photometric_interpretation", pixels.photometric_interpretation) < 0 ||
        set_string(dict, "transfer_syntax", pixels.transfer_syntax) < 0) {
      Py_DECREF(dict);
      return nullptr;
    }

    return dict;
  } catch (const std::exception& error) {
    return to_python_error(error);
  }
}

PyMethodDef methods[] = {
  {"read_tag", py_read_tag, METH_VARARGS, "Read a DICOM tag by group and element."},
  {"read_basic_metadata", py_read_basic_metadata, METH_VARARGS, "Read common DICOM metadata."},
  {"read_pixels", py_read_pixels, METH_VARARGS, "Read decompressed DICOM pixel bytes and image metadata."},
  {nullptr, nullptr, 0, nullptr}
};

PyModuleDef module = {
  PyModuleDef_HEAD_INIT,
  "gdcm_connector",
  "Python bindings for the C++ GDCM connector.",
  -1,
  methods
};

}  // namespace

PyMODINIT_FUNC PyInit_gdcm_connector() {
  return PyModule_Create(&module);
}
