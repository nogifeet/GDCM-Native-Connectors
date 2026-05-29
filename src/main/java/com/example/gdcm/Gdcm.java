package com.example.gdcm;

import java.util.Map;

public final class Gdcm {
    static {
        System.loadLibrary("gdcm_connector_jni");
    }

    private Gdcm() {
    }

    public static String readTag(String filePath, int group, int element) {
        return readTagNative(filePath, group, element);
    }

    public static Map<String, String> readBasicMetadata(String filePath) {
        return readBasicMetadataNative(filePath);
    }

    public static PixelData readPixels(String filePath) {
        return readPixelsNative(filePath);
    }

    private static native String readTagNative(String filePath, int group, int element);

    private static native Map<String, String> readBasicMetadataNative(String filePath);

    private static native PixelData readPixelsNative(String filePath);
}
