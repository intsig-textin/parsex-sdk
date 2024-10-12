package com.textinparsex.model;

import lombok.Data;

import java.util.List;

@Data
public class ImageBlock {
    private String type;
    private List<Integer> pos;
    private int zorder;
    private List<Integer> content;

    public ImageBlock(String type, List<Integer> pos, int zorder, List<Integer> content) {
        this.type = type;
        this.pos = pos;
        this.zorder = zorder;
        this.content = content;
    }

    // Getters and setters
    // ...
}