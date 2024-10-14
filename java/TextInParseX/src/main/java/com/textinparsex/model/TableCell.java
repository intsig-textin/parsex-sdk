package com.textinparsex.model;

import lombok.Data;

import java.util.List;

@Data
public class TableCell {
    private int row;
    private int col;
    private List<Integer> pos;
    private List<Object> content; // Can be TextBlock or ImageBlock
    private int rowSpan;
    private int colSpan;

    public TableCell(int row, int col, List<Integer> pos, List<Object> content, int rowSpan, int colSpan) {
        this.row = row;
        this.col = col;
        this.pos = pos;
        this.content = content;
        this.rowSpan = rowSpan;
        this.colSpan = colSpan;
    }

    // Getters and setters
    // ...
}