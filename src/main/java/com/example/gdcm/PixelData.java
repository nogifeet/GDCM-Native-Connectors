package com.example.gdcm;

public final class PixelData {
    private final byte[] data;
    private final int[] dimensions;
    private final int samplesPerPixel;
    private final int bitsAllocated;
    private final int bitsStored;
    private final int highBit;
    private final int pixelRepresentation;
    private final String scalarType;
    private final String photometricInterpretation;
    private final String transferSyntax;

    PixelData(
            byte[] data,
            int[] dimensions,
            int samplesPerPixel,
            int bitsAllocated,
            int bitsStored,
            int highBit,
            int pixelRepresentation,
            String scalarType,
            String photometricInterpretation,
            String transferSyntax) {
        this.data = data;
        this.dimensions = dimensions;
        this.samplesPerPixel = samplesPerPixel;
        this.bitsAllocated = bitsAllocated;
        this.bitsStored = bitsStored;
        this.highBit = highBit;
        this.pixelRepresentation = pixelRepresentation;
        this.scalarType = scalarType;
        this.photometricInterpretation = photometricInterpretation;
        this.transferSyntax = transferSyntax;
    }

    public byte[] getData() {
        return data;
    }

    public int[] getDimensions() {
        return dimensions;
    }

    public int getSamplesPerPixel() {
        return samplesPerPixel;
    }

    public int getBitsAllocated() {
        return bitsAllocated;
    }

    public int getBitsStored() {
        return bitsStored;
    }

    public int getHighBit() {
        return highBit;
    }

    public int getPixelRepresentation() {
        return pixelRepresentation;
    }

    public String getScalarType() {
        return scalarType;
    }

    public String getPhotometricInterpretation() {
        return photometricInterpretation;
    }

    public String getTransferSyntax() {
        return transferSyntax;
    }
}
