package com.textinparsex.model;

import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

import lombok.Data;
import org.opencv.core.Mat;

@Data
public class Document {
    private String version;
    private int duration;
    private Map<String, Object> result;
    private Metrics metrics;

    public Document(String version, int duration, Map<String, Object> result, Metrics metrics) {
        this.version = version;
        this.duration = duration;
        this.result = result;
        this.metrics = metrics;
    }

    public List<Page> getPages() {
        return (List<Page>) result.get("pages");
    }

    public List<ContentImage> getAllImages() {
        return getPages().stream()
                .flatMap(page -> page.getImages().stream())
                .collect(Collectors.toList());
    }

    public List<Mat> getAllImagesCvMat() {
        return getAllImages().stream()
                .map(image -> {
                    try {
                        return image.getData().toCvMat();
                    } catch (IllegalArgumentException e) {
                        System.err.println("Error processing image: " + e.getMessage());
                        return null;
                    }
                })
                .filter(mat -> mat != null)
                .collect(Collectors.toList());
    }

    public String getAllText() {
        return getPages().stream()
                .map(Page::getParagraphText)
                .collect(Collectors.joining());
    }

    public List<Table> getAllTables() {
        return getPages().stream()
                .flatMap(page -> page.getTables().stream())
                .collect(Collectors.toList());
    }

    public List<Paragraph> getAllParagraphs() {
        return getPages().stream()
                .flatMap(page -> page.getParagraphs().stream())
                .collect(Collectors.toList());
    }

    public String getAllMarkdown() {
        return (String) result.get("markdown");
    }

    // Getters and setters
    // ...
}