package com.textinparsex.model;

import lombok.Data;

import java.util.List;

@Data
public class ContentTextLine {
    private int id;
    private String type;
    private String text;
    private List<Integer> pos;
    private int angle;
    private String subType;
    private int direction;
    private Double score;
    private List<List<Integer>> charPos;
    private List<List<String>> charCand;
    private List<List<Double>> charCandScore;

    public ContentTextLine(int id, String type, String text, List<Integer> pos, int angle, String subType,
                           int direction, Double score, List<List<Integer>> charPos,
                           List<List<String>> charCand, List<List<Double>> charCandScore) {
        this.id = id;
        this.type = type;
        this.text = text;
        this.pos = pos;
        this.angle = angle;
        this.subType = subType;
        this.direction = direction;
        this.score = score;
        this.charPos = charPos;
        this.charCand = charCand;
        this.charCandScore = charCandScore;
    }

    public ContentTextLine() {}
}