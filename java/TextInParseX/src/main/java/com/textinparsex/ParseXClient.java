package com.textinparsex;

import com.textinparsex.model.*;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.textinparsex.model.Table;
import org.apache.http.client.methods.CloseableHttpResponse;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.ByteArrayEntity;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClients;
import org.apache.http.util.EntityUtils;
import org.opencv.core.Mat;

import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;
import java.util.stream.Collectors;

import org.apache.poi.ss.usermodel.*;
import org.apache.poi.xssf.usermodel.XSSFWorkbook;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ParseXClient {
    private String appId;
    private String secretCode;
    private Document priDocument;
    private Map<String, Object> result;
    private ObjectMapper objectMapper;

    private static final Logger logger = LoggerFactory.getLogger(ParseXClient.class);

    private final static String API_URI = "https://api.textin.com/ai/service/v1/pdf_to_markdown?markdown_details=1&apply_document_tree=1&page_details=1";

    public ParseXClient() {
        this(null, null);
    }

    public ParseXClient(String appId, String secretCode) {
        this.appId = appId;
        this.secretCode = secretCode;
        this.priDocument = new Document("1.0.0", 0, new HashMap<>(), new Metrics("unknown", 0, 0, 0));
        this.result = new HashMap<>();
        this.objectMapper = new ObjectMapper();
    }

    public Mat downloadImageFromUrl(String downloadUrl, String imageId) {
        String fullUrl = downloadUrl + "image_id=" + imageId;
        try (CloseableHttpClient httpClient = HttpClients.createDefault()) {
            HttpPost httpPost = new HttpPost(fullUrl);
            httpPost.setHeader("x-ti-app-id", appId);
            httpPost.setHeader("x-ti-secret-code", secretCode);

            try (CloseableHttpResponse response = httpClient.execute(httpPost)) {
                String jsonResponse = EntityUtils.toString(response.getEntity());
                Map<String, Object> result = objectMapper.readValue(jsonResponse, Map.class);

                if ((int) result.get("code") != 200) {
                    logger.error("API response error: {}", result);
                    return null;
                }

                Map<String, String> data = (Map<String, String>) result.get("data");
                String base64Image = data.get("image");
                ImageData imageData = new ImageData(base64Image, null, null);
                return imageData.toCvMat();
            }
        } catch (Exception e) {
            logger.error("Failed to download image from {}: {}", fullUrl, e.getMessage(), e);
            return null;
        }
    }

    public boolean checkVersion(String version) {
        String[] apiVersions = version.split("\\.");
        int[] standardVersion = {3, 6, 6};
        boolean flag = true;

        if (apiVersions.length < 3) {
            flag = false;
        } else {
            for (int i = 0; i < 3; i++) {
                int apiVersion = Integer.parseInt(apiVersions[i]);
                if (apiVersion < standardVersion[i]) {
                    flag = false;
                    break;
                } else if (apiVersion > standardVersion[i]) {
                    break;
                }
            }
        }

        if (!flag) {
            throw new RuntimeException("Wrong API version " + version + ", should be equal or greater than 3.6.6");
        }
        return flag;
    }

    public Document beginAnalyzeDocumentFromUrl(String filePath) throws IOException {
        return beginAnalyzeDocumentFromUrl(filePath, API_URI);
    }

    public Document beginAnalyzeDocumentFromUrl(String filePath, String apiUrl) throws IOException {
        try (CloseableHttpClient httpClient = HttpClients.createDefault()) {
            String paramToAdd = "from_sdk=java";
            if (apiUrl.contains("?")) {
                apiUrl = apiUrl.endsWith("&") ? apiUrl + paramToAdd : apiUrl + "&" + paramToAdd;
            } else {
                apiUrl += "?" + paramToAdd;
            }

            HttpPost httpPost = new HttpPost(apiUrl);
            httpPost.setHeader("x-ti-app-id", appId);
            httpPost.setHeader("x-ti-secret-code", secretCode);

            byte[] imageData = Files.readAllBytes(Paths.get(filePath));
            httpPost.setEntity(new ByteArrayEntity(imageData));

            try (CloseableHttpResponse response = httpClient.execute(httpPost)) {
                String jsonResponse = EntityUtils.toString(response.getEntity());
                Map<String, Object> result = objectMapper.readValue(jsonResponse, Map.class);

                if ((int) result.get("code") != 200) {
                    throw new RuntimeException("Code result error " + result);
                }

               checkVersion((String) result.get("version"));

                this.result = result;
                this.priDocument = parseXToMarkdownOutput(this.result);
                return this.priDocument;
            }
        } catch (Exception e) {
            throw new RuntimeException("Error occurred while analyzing document: " + e.getMessage(), e);
        }
    }

    public Document beginAnalyzeDocumentFromJson(Map<String, Object> jsonObject) {
        checkVersion((String) jsonObject.get("version"));
        this.priDocument = parseXToMarkdownOutput(jsonObject);
        return this.priDocument;
    }

    public Document beginAnalyzeDocumentFromFile(String jsonPath) throws IOException {
        Map<String, Object> data = objectMapper.readValue(Paths.get(jsonPath).toFile(), Map.class);
        checkVersion((String) data.get("version"));
        logger.info("Starting document parsing from file: {}", jsonPath);
        this.priDocument = parseXToMarkdownOutput(data);
        return this.priDocument;
    }

    public void printAllElements(Object obj) {
        printAllElements(obj, 0, 50);
    }

    public void printAllElements(Object obj, int indent, int maxStrLength) {
        String indentSpace = String.join("", Collections.nCopies(indent, "  "));

        if (obj instanceof List) {
            List<?> list = (List<?>) obj;
            if (!list.isEmpty() && list.get(0) instanceof Integer) {
                logger.info("{}{}", indentSpace, list);
            } else {
                for (int i = 0; i < list.size(); i++) {
                    logger.info("{}[{}]", indentSpace, i);
                    printAllElements(list.get(i), indent + 1, maxStrLength);
                }
            }
        } else if (obj instanceof Map) {
            Map<?, ?> map = (Map<?, ?>) obj;
            for (Map.Entry<?, ?> entry : map.entrySet()) {
                if (entry.getValue() instanceof List || entry.getValue() instanceof Map) {
                    if (entry.getValue() instanceof List && !((List<?>) entry.getValue()).isEmpty() && ((List<?>) entry.getValue()).get(0) instanceof Integer) {
                        logger.info("{}{}: {}", indentSpace, entry.getKey(), entry.getValue());
                    } else {
                        logger.info("{}{}:", indentSpace, entry.getKey());
                        printAllElements(entry.getValue(), indent + 1, maxStrLength);
                    }
                } else {
                    logger.info("{}{}: {}", indentSpace, entry.getKey(), 
                        formatValue(entry.getValue(), maxStrLength));
                }
            }
        } else {
            logger.info("{}{}", indentSpace, formatValue(obj, maxStrLength));
        }
    }

    private String formatValue(Object value, int maxLength) {
        if (value instanceof String) {
            String strValue = (String) value;
            if (strValue.length() > maxLength) {
                return strValue.substring(0, maxLength) + "...";
            }
        }
        return String.valueOf(value);
    }

    private Document parseXToMarkdownOutput(Map<String, Object> data) {
        // This method needs to be implemented based on your specific requirements
        // It should create a Document object from the parsed JSON data
        // Here's a basic structure:

        Map<String, Object> resultData = (Map<String, Object>) data.get("result");
        List<Page> pages = parsePages(resultData);

        Object metricsObject = data.get("metrics");
        Metrics metrics;
        if (metricsObject instanceof Map) {
            // 如果 metrics 是 Map 类型，调用 parseMetrics
            Map<String, Object> metricsData = (Map<String, Object>) metricsObject;
            metrics = parseMetrics(metricsData);
        } else {
            metrics = new Metrics("unknown", 0, 0, 0);
        }

        Map<String, Object> result = new HashMap<>();
        result.put("pages", pages);
        result.put("details", resultData.get("detail"));
        result.put("markdown", resultData.get("markdown"));

        return new Document(
                (String) data.get("version"),
                (int) data.get("duration"),
                result,
                metrics
        );
    }

    private List<Page> parsePages(Map<String, Object> resultData) {
        List<Map<String, Object>> pagesData = (List<Map<String, Object>>) resultData.getOrDefault("pages", new ArrayList<>());
        List<Map<String, Object>> detailsData = (List<Map<String, Object>>) resultData.getOrDefault("detail", new ArrayList<>());
        String markdown = (String) resultData.getOrDefault("markdown", "");

        List<Page> pages = new ArrayList<>();

        for (Map<String, Object> pageData : pagesData) {
            String status = (String) pageData.get("status");
            int pageId = (int) pageData.get("page_id");
            double durations = ((Number) pageData.get("durations")).doubleValue();
            String imageId = (String) pageData.get("image_id");
            Integer width = (Integer) pageData.get("width");
            Integer height = (Integer) pageData.get("height");
            int angle = (int) pageData.getOrDefault("angle", 0);
            int num = (int) pageData.getOrDefault("num", 0);

            ImageData image = null;
            if (pageData.containsKey("image")) {
                Map<String, Object> imageData = (Map<String, Object>) pageData.get("image");
                image = new ImageData(
                        (String) imageData.get("base64"),
                        (List<Integer>) imageData.get("region"),
                        (String) imageData.get("path")
                );
            }

            List<Object> content = parseContent((List<Map<String, Object>>) pageData.get("content"));
            List<Table> structured = parseTables((List<Map<String, Object>>) pageData.get("structured"));
            List<Paragraph> structuredPara = parseParagraphs((List<Map<String, Object>>) pageData.get("structured"));

            List<Map<String, Object>> markdownDetails = detailsData.stream()
                    .filter(md -> (int) md.get("page_id") == pageId)
                    .collect(Collectors.toList());

            Page page = new Page(status, pageId, durations, imageId, width, height, angle, num, image,
                    content, structured, structuredPara, markdownDetails);
            pages.add(page);
        }

        return pages;
    }

    private List<Object> parseContent(List<Map<String, Object>> contentData) {
        List<Object> content = new ArrayList<>();
        for (Map<String, Object> item : contentData) {
            String type = (String) item.get("type");
            if ("line".equals(type)) {
                Object scoreObject = item.get("score");
                Double score;
                if (scoreObject instanceof Number) {
                    score = ((Number) scoreObject).doubleValue();
                } else {
                    score = 0.0;
                }
                content.add(new ContentTextLine(
                        (int) item.get("id"),
                        type,
                        (String) item.get("text"),
                        (List<Integer>) item.get("pos"),
                        (int) item.getOrDefault("angle", 0),
                        (String) item.get("sub_type"),
                        (int) item.getOrDefault("direction", 0),
                        score,
                        (List<List<Integer>>) item.get("char_pos"),
                        (List<List<String>>) item.get("char_cand"),
                        (List<List<Double>>) item.get("char_cand_score")
                ));
            } else if ("image".equals(type)) {
                Map<String, Object> dataMap = (Map<String, Object>) item.get("data");
                ImageData imageData = new ImageData(
                        (String) dataMap.get("base64"),
                        (List<Integer>) dataMap.get("region"),
                        (String) dataMap.get("path")
                );
                content.add(new ContentImage(
                        (int) item.get("id"),
                        type,
                        (List<Integer>) item.get("pos"),
                        imageData,
                        (String) item.get("sub_type"),
                        (String) item.get("stamp_type"),
                        (String) item.get("stamp_shape"),
                        (String) item.get("stamp_color"),
                        (List<Integer>) item.get("size")
                ));
            }
        }
        return content;
    }

    private List<Table> parseTables(List<Map<String, Object>> structuredData) {
        return structuredData.stream()
                .filter(s -> "table".equals(s.get("type")))
                .map(this::parseTable)
                .collect(Collectors.toList());
    }

    private Table parseTable(Map<String, Object> tableData) {
        List<TableCell> cells = ((List<Map<String, Object>>) tableData.get("cells")).stream()
                .map(this::parseTableCell)
                .collect(Collectors.toList());

        return new Table(
                (String) tableData.get("type"),
                (List<Integer>) tableData.get("pos"),
                (int) tableData.get("rows"),
                (int) tableData.get("cols"),
                (List<Integer>) tableData.get("columns_width"),
                (List<Integer>) tableData.get("rows_height"),
                cells,
                (String) tableData.get("sub_type"),
                (boolean) tableData.getOrDefault("is_continue", false),
                (String) tableData.get("parse_type")
        );
    }

    private TableCell parseTableCell(Map<String, Object> cellData) {
        List<Object> content = ((List<Map<String, Object>>) cellData.get("content")).stream()
                .map(this::parseTableCellContent)
                .collect(Collectors.toList());

        return new TableCell(
                (int) cellData.get("row"),
                (int) cellData.get("col"),
                (List<Integer>) cellData.get("pos"),
                content,
                (int) cellData.getOrDefault("row_span", 1),
                (int) cellData.getOrDefault("col_span", 1)
        );
    }

    private Object parseTableCellContent(Map<String, Object> contentData) {
        String type = (String) contentData.get("type");
        if ("textblock".equals(type)) {
            return new TextBlock(
                    type,
                    (List<Integer>) contentData.get("pos"),
                    (List<Object>) contentData.get("content"),
                    (String) contentData.get("sub_type"),
                    (boolean) contentData.getOrDefault("is_continue", false)
            );
        } else if ("imageblock".equals(type)) {
            return new ImageBlock(
                    type,
                    (List<Integer>) contentData.get("pos"),
                    (int) contentData.get("zorder"),
                    (List<Object>) contentData.get("content")
            );
        }
        return null;
    }

    private List<Paragraph> parseParagraphs(List<Map<String, Object>> structuredData) {
        return structuredData.stream()
                .filter(s -> "textblock".equals(s.get("type")))
                .map(this::parseParagraph)
                .collect(Collectors.toList());
    }

    private Paragraph parseParagraph(Map<String, Object> paragraphData) {
        return new Paragraph(
                (List<Integer>) paragraphData.get("pos"),
                (List<Object>) paragraphData.get("content")
        );
    }

    private Metrics parseMetrics(Map<String, Object> metricsData) {
        return new Metrics(
                (String) metricsData.get("document_type"),
                (int) metricsData.get("valid_page_number"),
                (int) metricsData.get("paragraph_number"),
                (int) metricsData.get("character_number")
        );
    }

    public void saveTablesAsExcel(List<Table> tables, String filePath) throws IOException {
        try (Workbook workbook = new XSSFWorkbook()) {
            for (int i = 0; i < tables.size(); i++) {
                Table table = tables.get(i);
                Sheet sheet = workbook.createSheet("Table " + (i + 1));
                
                for (int rowIndex = 0; rowIndex < table.getRows(); rowIndex++) {
                    Row row = sheet.createRow(rowIndex);
                    final int finalRowIndex = rowIndex;
                    for (int colIndex = 0; colIndex < table.getCols(); colIndex++) {
                        Cell cell = row.createCell(colIndex);
                        final int finalColIndex = colIndex;
                        TableCell tableCell = table.getCells().stream()
                                .filter(c -> c.getRow() == finalRowIndex && c.getCol() == finalColIndex)
                                .findFirst()
                                .orElse(null);
                        
                        if (tableCell != null) {
                            String cellContent = getCellContent(tableCell);
                            cell.setCellValue(cellContent);
                        }
                    }
                }
                
                // Auto-size columns
                for (int colIndex = 0; colIndex < table.getCols(); colIndex++) {
                    sheet.autoSizeColumn(colIndex);
                }
            }
            
            // Write the workbook to a file
            try (FileOutputStream fileOut = new FileOutputStream(filePath)) {
                workbook.write(fileOut);
            }
        }
    }

    private String getCellContent(TableCell tableCell) {
        StringBuilder content = new StringBuilder();
        for (Object item : tableCell.getContent()) {
            if (item instanceof TextBlock) {
                TextBlock textBlock = (TextBlock) item;
                content.append(textBlock.getContent().stream()
                        .map(this::getContentText)
                        .collect(Collectors.joining(" ")));
            } else if (item instanceof ImageBlock) {
                content.append("[Image]");
            }
            content.append(" ");
        }
        return content.toString().trim();
    }

    private String getContentText(Object contentItem) {
        if (contentItem instanceof ContentTextLine) {
            return ((ContentTextLine) contentItem).getText();
        } else if (contentItem instanceof ContentImage) {
            return "[Image]";
        } else {
            return contentItem.toString();
        }
    }

    
}
