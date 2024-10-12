package com.textinparsex.model;

import lombok.Data;

import java.util.List;

@Data
public class Paragraph {
    private List<Integer> pos;
    private List<Object> lines;

    public Paragraph(List<Integer> pos, List<Object> content) {
        this.pos = pos;
        this.lines = content;
    }

    // Getters and setters
    // ...
}