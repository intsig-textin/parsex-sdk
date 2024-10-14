package com.textinparsex.model;

import lombok.Data;

import java.util.List;

@Data
public class Table {
    private String type;
    private List<Integer> pos;
    private int rows;
    private int cols;
    private List<Integer> columnsWidth;
    private List<Integer> rowsHeight;
    private List<TableCell> cells;
    private String subType;
    private boolean isContinue;
    private String parseType;

    public Table(String type, List<Integer> pos, int rows, int cols,
                 List<Integer> columnsWidth, List<Integer> rowsHeight,
                 List<TableCell> cells, String subType, boolean isContinue, String parseType) {
        this.type = type;
        this.pos = pos;
        this.rows = rows;
        this.cols = cols;
        this.columnsWidth = columnsWidth;
        this.rowsHeight = rowsHeight;
        this.cells = cells;
        this.subType = subType;
        this.isContinue = isContinue;
        this.parseType = parseType;
    }

    // Getters and setters
    // ...
}