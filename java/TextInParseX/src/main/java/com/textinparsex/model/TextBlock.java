package com.textinparsex.model;

import lombok.Data;

import java.util.List;

@Data
public class TextBlock {
    private String type;
    private List<Integer> pos;
    private List<Object> content;
    private String subType;
    private boolean isContinue;

    public TextBlock(String type, List<Integer> pos, List<Object> content, String subType, boolean isContinue) {
        this.type = type;
        this.pos = pos;
        this.content = content;
        this.subType = subType;
        this.isContinue = isContinue;
    }

    // Getters and setters
    // ...
}