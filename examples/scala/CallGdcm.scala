import com.example.gdcm.Gdcm

import java.util.Arrays

object CallGdcmScala {
  def main(args: Array[String]): Unit = {
    if (args.length != 1) {
      Console.err.println("usage: scala CallGdcmScala /path/to/file.dcm")
      sys.exit(2)
    }

    val filePath = args(0)

    val modality = Gdcm.readTag(filePath, 0x0008, 0x0060)
    println("readTag:")
    println("  Modality: " + modality)

    val metadata = Gdcm.readBasicMetadata(filePath)
    println("readBasicMetadata:")
    println("  Patient name: " + metadata.get("patient_name"))
    println("  Patient ID: " + metadata.get("patient_id"))
    println("  Study UID: " + metadata.get("study_instance_uid"))
    println("  Series UID: " + metadata.get("series_instance_uid"))
    println("  SOP UID: " + metadata.get("sop_instance_uid"))
    println("  Modality: " + metadata.get("modality"))
    println("  Transfer syntax: " + metadata.get("transfer_syntax"))

    val pixels = Gdcm.readPixels(filePath)
    println("readPixels:")
    println("  Dimensions: " + Arrays.toString(pixels.getDimensions()))
    println("  Samples per pixel: " + pixels.getSamplesPerPixel())
    println("  Bits allocated: " + pixels.getBitsAllocated())
    println("  Bits stored: " + pixels.getBitsStored())
    println("  High bit: " + pixels.getHighBit())
    println("  Pixel representation: " + pixels.getPixelRepresentation())
    println("  Scalar type: " + pixels.getScalarType())
    println("  Photometric interpretation: " + pixels.getPhotometricInterpretation())
    println("  Transfer syntax: " + pixels.getTransferSyntax())
    println("  Pixel byte length: " + pixels.getData().length)
  }
}
