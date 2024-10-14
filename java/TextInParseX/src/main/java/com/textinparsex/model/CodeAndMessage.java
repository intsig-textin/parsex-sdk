package com.textinparsex.model;

import lombok.Data;

@Data
public class CodeAndMessage {
    private int code;
    private String message;

    public CodeAndMessage(int code, String message) {
        this.code = code;
        this.message = message;
    }

    // Getters and setters
    // ...
}