import com.example.gdcm.Gdcm;
import com.example.gdcm.PixelData;

import java.util.Arrays;
import java.util.Map;

public final class CallGdcm {
    private CallGdcm() {
    }

    public static void main(String[] args) {
        if (args.length != 1) {
            System.err.println("usage: java CallGdcm /path/to/file.dcm");
            System.exit(2);
        }

        String filePath = args[0];

        testReadTag(filePath);
        testReadBasicMetadata(filePath);
        testReadPixels(filePath);
    }

    private static void testReadTag(String filePath) {
        String modality = Gdcm.readTag(filePath, 0x0008, 0x0060);
        System.out.println("readTag:");
        System.out.println("  Modality: " + modality);
    }

    private static void testReadBasicMetadata(String filePath) {
        Map<String, String> metadata = Gdcm.readBasicMetadata(filePath);
        System.out.println("readBasicMetadata:");
        System.out.println("  Patient name: " + metadata.get("patient_name"));
        System.out.println("  Patient ID: " + metadata.get("patient_id"));
        System.out.println("  Study UID: " + metadata.get("study_instance_uid"));
        System.out.println("  Series UID: " + metadata.get("series_instance_uid"));
        System.out.println("  SOP UID: " + metadata.get("sop_instance_uid"));
        System.out.println("  Modality: " + metadata.get("modality"));
        System.out.println("  Transfer syntax: " + metadata.get("transfer_syntax"));
    }

    private static void testReadPixels(String filePath) {
        PixelData pixels = Gdcm.readPixels(filePath);
        System.out.println("readPixels:");
        System.out.println("  Dimensions: " + Arrays.toString(pixels.getDimensions()));
        System.out.println("  Samples per pixel: " + pixels.getSamplesPerPixel());
        System.out.println("  Bits allocated: " + pixels.getBitsAllocated());
        System.out.println("  Bits stored: " + pixels.getBitsStored());
        System.out.println("  High bit: " + pixels.getHighBit());
        System.out.println("  Pixel representation: " + pixels.getPixelRepresentation());
        System.out.println("  Scalar type: " + pixels.getScalarType());
        System.out.println("  Photometric interpretation: " + pixels.getPhotometricInterpretation());
        System.out.println("  Transfer syntax: " + pixels.getTransferSyntax());
        System.out.println("  Pixel byte length: " + pixels.getData().length);
    }
}
