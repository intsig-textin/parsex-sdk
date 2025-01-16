
import com.textinparsex.ParseXClient;
import com.textinparsex.model.*;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

import java.util.List;

public class TestSDK {

    public static final String OPENCV_PATH = "/your/path/to/opencv/lib/libopencv_java455.so";
    public static final String API_KEY = "********";
    public static final String API_SECRET = "********";
    public static final String PDF_FILE_PATH = "/your/path/to/example.pdf";
    public static final String API_URL = "https://api.textin.com/ai/service/v1/pdf_to_markdown?markdown_details=1&apply_document_tree=1&page_details=1&get_image=both";
    public static final String DOWNLOAD_IMAGE_URL = "https://api.textin.com/ocr_image/download?";

    public static void main(String[] args) throws Exception {
        System.load(OPENCV_PATH);

        ParseXClient parseXClient = new ParseXClient(API_KEY, API_SECRET);

        String pdfFilePath = PDF_FILE_PATH;

        Document result = parseXClient.beginAnalyzeDocumentFromUrl(pdfFilePath, API_URL);

        System.out.println("markdown: " + result.getAllMarkdown());

        System.out.println("All text in document:");
        parseXClient.printAllElements(result.getAllText(), 0, 1000);

        System.out.println("Total tables in document: ");
        for (Table table : result.getAllTables()) {
            parseXClient.printAllElements(table);
        }

        System.out.println("Total paragraphs in document: ");
        List<Paragraph> paragraphs = result.getAllParagraphs();
        for (int pIdx = 0; pIdx < paragraphs.size(); pIdx++) {
            Paragraph eachParagraph = paragraphs.get(pIdx);
            System.out.println("\n--- Paragraph " + pIdx + "/" + paragraphs.size() + " ---");
            System.out.println("Paragraph position: " + eachParagraph.getPos());

            List<Object> lines = eachParagraph.getLines();
            for (int lIdx = 0; lIdx < lines.size(); lIdx++) {
                Object eachLine = lines.get(lIdx);
                System.out.println("  Line " + lIdx + "/" + lines.size());
                if (eachLine instanceof ContentTextLine) {
                    System.out.println("    Line positions: " + ((ContentTextLine)eachLine).getPos());
                    System.out.println("  Line text: " + ((ContentTextLine)eachLine).getText());
                }
            }
        }

        System.out.println("Total images in document: ");
        List<ContentImage> images = result.getAllImages();
        for (int i = 0; i < images.size(); i++) {
            System.out.println("Image: " + i);
            parseXClient.printAllElements(images.get(i));
        }

        System.out.println("Total images (as cv Mat) in document: ");
        List<Mat> cvImages = result.getAllImagesCvMat();
        for (int i = 0; i < cvImages.size(); i++) {
            System.out.println("Image (as cv Mat): " + i);
            parseXClient.printAllElements(cvImages.get(i));
        }

        for (Page page : result.getPages()) {
            Mat pageImg = parseXClient.downloadImageFromUrl(DOWNLOAD_IMAGE_URL, page.getImageId());
            if (pageImg == null) continue;

            // 获取表格图片
            for (int i = 0; i < page.getTables().size(); i++) {
                Table table = page.getTables().get(i);
                int x1 = table.getPos().get(0);
                int y1 = table.getPos().get(1);
                int x2 = table.getPos().get(4);
                int y2 = table.getPos().get(5);
                
                Mat tableImg = new Mat(pageImg, 
                    new org.opencv.core.Rect(x1, y1, x2 - x1, y2 - y1));
                Imgcodecs.imwrite(String.format("page_%d_table_%d.jpg", 
                    page.getPageId(), i), tableImg);
            }

            for (Table table : page.getTables()) {
                for (TableCell cell : table.getCells()) {
                    Imgproc.rectangle(pageImg,
                            new Point(cell.getPos().get(0), cell.getPos().get(1)),
                            new Point(cell.getPos().get(4), cell.getPos().get(5)),
                            new Scalar(0, 0, 255), 1);
                }
            }

            for (ContentImage image : page.getImages()) {
                Imgproc.rectangle(pageImg,
                        new Point(image.getPos().get(0), image.getPos().get(1)),
                        new Point(image.getPos().get(4), image.getPos().get(5)),
                        new Scalar(0, 255, 255), 1);
            }

            for (Paragraph paragraph : page.getParagraphs()) {
                Imgproc.rectangle(pageImg,
                        new Point(paragraph.getPos().get(0), paragraph.getPos().get(1)),
                        new Point(paragraph.getPos().get(4), paragraph.getPos().get(5)),
                        new Scalar(0, 255, 0), 5);

                for (Object line : paragraph.getLines()) {
                    if (line instanceof ContentTextLine) {
                        ContentTextLine textLine = (ContentTextLine) line;
                        Imgproc.rectangle(pageImg,
                                new Point(textLine.getPos().get(0), textLine.getPos().get(1)),
                                new Point(textLine.getPos().get(4), textLine.getPos().get(5)),
                                new Scalar(255, 0, 0), 1);
                    }
                }
            }

            Imgcodecs.imwrite("image_result_" + page.getPageId() + ".jpg", pageImg);
        }

        // convert to execl
        List<Table> tableList = result.getAllTables();
        parseXClient.saveTablesAsExcel(tableList, "/your/path/to/example.xlsx");

    }

}
