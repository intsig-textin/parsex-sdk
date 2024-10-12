package com.textinparsex.model;

import lombok.Data;

@Data
public class Metrics {
    private String documentType;
    private int validPageNumber;
    private int paragraphNumber;
    private int characterNumber;

    public Metrics(String documentType, int validPageNumber, int paragraphNumber, int characterNumber) {
        this.documentType = documentType;
        this.validPageNumber = validPageNumber;
        this.paragraphNumber = paragraphNumber;
        this.characterNumber = characterNumber;
    }

    // Getters and setters
    // ...
}