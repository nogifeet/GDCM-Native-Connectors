# GDCM Native Connectors

Minimal native connectors for calling [GDCM](https://github.com/malaterre/GDCM)
from C++, Python, Java, and Scala.

This project exposes a small C++ wrapper around GDCM and binds it to:

- Python through a native CPython extension named `gdcm_connector`
- Java through JNI
- Scala through the Java API

## Status

This is an experimental starter connector, not a production SDK. It is useful
for learning and for proving the native binding path across Python, Java, and
Scala.

Current API:

- `read_tag(file_path, group, element)`
- `read_basic_metadata(file_path)`
- `read_pixels(file_path)`

`read_pixels` returns decompressed pixel bytes plus image metadata such as
dimensions, samples per pixel, bits allocated, scalar type, photometric
interpretation, and transfer syntax.

## Tested Platform

Tested locally with:

- macOS 26.5 arm64
- Apple Clang 21.0.0
- CMake 4.3.1
- GDCM 3.2.6 from Homebrew
- Python 3.13.5 from Anaconda
- OpenJDK 8, Zulu 1.8.0_292
- Scala 2.12.20

Other platforms should work with the same dependencies, but Linux and Windows
are not yet verified.

## Requirements

- CMake 3.20+
- A C++17 compiler
- GDCM with CMake package files
- Python 3 development headers, if building Python bindings
- JDK, if building Java/Scala bindings
- Scala compiler/runtime, if running the Scala example

On macOS with Homebrew:

```sh
brew install gdcm cmake
```

If CMake cannot find GDCM automatically, pass the GDCM CMake package directory:

```sh
cmake -S . -B build -DGDCM_DIR=/path/to/gdcm/lib/cmake/gdcm
```

## Build

```sh
cmake -S . -B build
cmake --build build
```

Disable one binding when needed:

```sh
cmake -S . -B build -DGDCM_CONNECTOR_BUILD_PYTHON=OFF
cmake -S . -B build -DGDCM_CONNECTOR_BUILD_JAVA=OFF
```

## Python Notebook

The Python binding is built as:

```text
build/python/gdcm_connector.so
```

In a notebook, add the build output directory to `sys.path` before importing:

```python
import sys

sys.path.insert(0, "/Users/nitin/Documents/Connector_C++/build/python")

import gdcm_connector
```

Then call the methods:

```python
file_path = "/Users/nitin/Documents/Connector_C++/dicom_samples/1-01.dcm"

metadata = gdcm_connector.read_basic_metadata(file_path)
modality = gdcm_connector.read_tag(file_path, 0x0008, 0x0060)
pixels = gdcm_connector.read_pixels(file_path)

raw_bytes = pixels["data"]
dimensions = pixels["dimensions"]
scalar_type = pixels["scalar_type"]
```

Convert pixel bytes to a NumPy array:

```python
import numpy as np

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
width, height = pixels["dimensions"][:2]
image = np.frombuffer(pixels["data"], dtype=dtype).reshape(height, width)
```

## Java

The Java API lives in `src/main/java/com/example/gdcm`. The native JNI library
is built at:

```text
build/java/libgdcm_connector_jni.dylib
```

Compile the Java API and example:

```sh
javac -d build/classes \
  src/main/java/com/example/gdcm/Gdcm.java \
  src/main/java/com/example/gdcm/PixelData.java \
  examples/java/CallGdcm.java
```

Run the Java example:

```sh
java -Djava.library.path=build/java -cp build/classes CallGdcm dicom_samples/1-01.dcm
```

Programmatic use:

```java
import com.example.gdcm.Gdcm;
import com.example.gdcm.PixelData;

import java.util.Map;

String filePath = "dicom_samples/1-01.dcm";

String modality = Gdcm.readTag(filePath, 0x0008, 0x0060);
Map<String, String> metadata = Gdcm.readBasicMetadata(filePath);
PixelData pixels = Gdcm.readPixels(filePath);

byte[] rawBytes = pixels.getData();
int[] dimensions = pixels.getDimensions();
String scalarType = pixels.getScalarType();
```

## Scala

Scala uses the same Java API; there is no separate Scala-native binding.

First compile the Java API:

```sh
javac -d build/classes \
  src/main/java/com/example/gdcm/Gdcm.java \
  src/main/java/com/example/gdcm/PixelData.java
```

Then compile the Scala example against those Java classes:

```sh
scalac -d build/classes -cp build/classes examples/scala/CallGdcm.scala
```

Run the Scala example:

```sh
scala -Djava.library.path=build/java -cp build/classes CallGdcmScala dicom_samples/1-01.dcm
```

Programmatic use:

```scala
import com.example.gdcm.Gdcm

val filePath = "dicom_samples/1-01.dcm"

val modality = Gdcm.readTag(filePath, 0x0008, 0x0060)
val metadata = Gdcm.readBasicMetadata(filePath)
val pixels = Gdcm.readPixels(filePath)

val rawBytes = pixels.getData()
val dimensions = pixels.getDimensions()
val scalarType = pixels.getScalarType()
```

## Verify

Build the native libraries:

```sh
cmake -S . -B build
cmake --build build
```

Verify Java:

```sh
javac -d build/classes \
  src/main/java/com/example/gdcm/Gdcm.java \
  src/main/java/com/example/gdcm/PixelData.java \
  examples/java/CallGdcm.java

java -Djava.library.path=build/java -cp build/classes CallGdcm dicom_samples/1-01.dcm
```

Verify Scala:

```sh
scalac -d build/classes -cp build/classes examples/scala/CallGdcm.scala
scala -Djava.library.path=build/java -cp build/classes CallGdcmScala dicom_samples/1-01.dcm
```

Expected sample output includes:

```text
Modality: PT
Dimensions: [128, 128]
Scalar type: INT16
Pixel byte length: 32768
```
