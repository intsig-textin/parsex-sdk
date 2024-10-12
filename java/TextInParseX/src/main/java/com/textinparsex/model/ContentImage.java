package com.textinparsex.model;

import lombok.Data;

import java.util.List;

@Data
public class ContentImage {
    private int id;
    private String type;
    private List<Integer> pos;
    private ImageData data;
    private String subType;
    private String stampType;
    private String stampShape;
    private String stampColor;
    private List<Integer> size;

    public ContentImage(int id, String type, List<Integer> pos, ImageData data, String subType,
                        String stampType, String stampShape, String stampColor, List<Integer> size) {
        this.id = id;
        this.type = type;
        this.pos = pos;
        this.data = data;
        this.subType = subType;
        this.stampType = stampType;
        this.stampShape = stampShape;
        this.stampColor = stampColor;
        this.size = size;
    }
}