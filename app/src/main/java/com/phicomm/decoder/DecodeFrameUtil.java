package com.phicomm.decoder;

public class DecodeFrameUtil {
    static {
        System.loadLibrary("decodeframe");
    }
    public static native int randomscreen(String inputurl,String outputurl);
}
