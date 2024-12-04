package com.textinparsex.model;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;
import org.opencv.core.Mat;
import lombok.Data;
import java.util.Collections;

@Data
public class Page {
    private String status;
    private int pageId;
    private double durations;
    private String imageId;
    private Integer width;
    private Integer height;
    private int angle;
    private int num;
    private ImageData image;
    private List<Object> content;
    private List<Table> structured;
    private List<Paragraph> structuredPara;
    private List<Map<String, Object>> markdownDetails;

    public Page(String status, int pageId, double durations, String imageId, Integer width, Integer height,
                int angle, int num, ImageData image, List<Object> content, List<Table> structured,
                List<Paragraph> structuredPara, List<Map<String, Object>> markdownDetails) {
        this.status = status;
        this.pageId = pageId;
        this.durations = durations;
        this.imageId = imageId;
        this.width = width;
        this.height = height;
        this.angle = angle;
        this.num = num;
        this.image = image;
        this.content = content;
        this.structured = structured;
        this.structuredPara = structuredPara;
        this.markdownDetails = markdownDetails;
    }

    public String getMarkdown() {
        StringBuilder result = new StringBuilder();
        for (Map<String, Object> s : markdownDetails) {
            StringBuilder paraStr = new StringBuilder();
            int outlineLevel = (int) s.get("outline_level");
            paraStr.append(String.join("", Collections.nCopies(outlineLevel + 1, "#")));
            if (paraStr.length() > 0) {
                paraStr.append(" ");
            }
            paraStr.append(s.get("text"));
            if (paraStr.length() > 0) {
                paraStr.append("\n\n");
            }
            result.append(paraStr);
        }
        return result.toString();
    }

    public List<Table> getTables() {
        Map<Integer, Object> contentMap = content.stream()
                .collect(Collectors.toMap(
                        item -> item instanceof ContentTextLine ? ((ContentTextLine) item).getId() : ((ContentImage) item).getId(),
                        item -> item
                ));

        return structured.stream()
                .map(table -> {
                    List<TableCell> newCells = table.getCells().stream()
                            .map(cell -> {
                                List<Object> newContent = cell.getContent().stream()
                                        .map(item -> {
                                            if (item instanceof TextBlock) {
                                                TextBlock textBlock = (TextBlock) item;
                                                List<Object> resolvedContent = textBlock.getContent().stream()
                                                        .map(id -> contentMap.get((Integer) id))
                                                        .collect(Collectors.toList());
                                                return new TextBlock(textBlock.getType(), textBlock.getPos(), resolvedContent, textBlock.getSubType(), textBlock.isContinue());
                                            } else if (item instanceof ImageBlock) {
                                                ImageBlock imageBlock = (ImageBlock) item;
                                                List<Object> resolvedContent = imageBlock.getContent().stream()
                                                        .map(id -> contentMap.get((Integer) id))
                                                        .collect(Collectors.toList());
                                                return new ImageBlock(imageBlock.getType(), imageBlock.getPos(), imageBlock.getZorder(), resolvedContent);
                                            }
                                            return item;
                                        })
                                        .collect(Collectors.toList());
                                return new TableCell(cell.getRow(), cell.getCol(), cell.getPos(), newContent, cell.getRowSpan(), cell.getColSpan());
                            })
                            .collect(Collectors.toList());
                    return new Table(table.getType(), table.getPos(), table.getRows(), table.getCols(),
                            table.getColumnsWidth(), table.getRowsHeight(), newCells, table.getSubType(),
                            table.isContinue(), table.getParseType());
                })
                .collect(Collectors.toList());
    }

    public List<Paragraph> getParagraphs() {
        Map<Integer, Object> contentMap = content.stream()
                .collect(Collectors.toMap(
                        item -> item instanceof ContentTextLine ? ((ContentTextLine) item).getId() : ((ContentImage) item).getId(),
                        item -> item
                ));

        return structuredPara.stream()
                .map(para -> {
                    List<Object> resolvedContent = para.getLines().stream()
                            .map(contentMap::get)
                            .collect(Collectors.toList());
                    return new Paragraph(para.getPos(), resolvedContent);
                })
                .collect(Collectors.toList());
    }

    public List<ContentImage> getImages() {
        return content.stream()
                .filter(item -> item instanceof ContentImage)
                .map(item -> (ContentImage) item)
                .collect(Collectors.toList());
    }

    public List<Mat> getImagesCvMat() {
        return getImages().stream()
                .map(image -> image.getData().toCvMat())
                .collect(Collectors.toList());
    }

    public String getParagraphText() {
        return content.stream()
                .filter(item -> item instanceof ContentTextLine)
                .map(item -> ((ContentTextLine) item).getText())
                .collect(Collectors.joining());
    }

    // Getters and setters
    // ...
}
