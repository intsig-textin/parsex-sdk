package com.textinparsex.model;

import lombok.Data;
import org.opencv.core.Mat;
import org.opencv.core.MatOfByte;
import org.opencv.imgcodecs.Imgcodecs;

import java.util.List;
import java.util.Base64;

@Data
public class ImageData {
    private String base64;
    private List<Integer> region;
    private String path;

    public ImageData(String base64, List<Integer> region, String path) {
        this.base64 = base64;
        this.region = region;
        this.path = path;
    }

    public Mat toCvMat() throws IllegalArgumentException {
        if (base64 != null) {
            byte[] imageData = Base64.getDecoder().decode(base64);
            Mat mat = Imgcodecs.imdecode(new MatOfByte(imageData), Imgcodecs.IMREAD_COLOR);
            if (mat.empty()) {
                throw new IllegalArgumentException("Cannot decode image from base64");
            }
            return mat;
        } else {
            throw new IllegalArgumentException("No valid image data found.");
        }
    }

    // Getters and setters
    // ...
}