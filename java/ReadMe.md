为了让用户获得文档解析引擎返回的丰富版面元素，我们开发了一系列的sdk函数，包括目录树、公式、表格、图片、全文markdown等结果的获取函数。一些场景中，部分用户希望通过api进行调用，还有一部分用户希望能够直接可视化文件解析的结果，且可以进行编辑修正，此次我们也将textin.com上面的前端可视化组件进行了开源，请用户批评指正。


# 1、SDK功能介绍
ParseX是一套标准的多平台支持的java sdk，帮助开发者解析pdf_to_markdownRestful API返回结果，获取对应的版面元素的数据结构。开发者只需下载jar包，并导入到自己的项目中即可使用。

# 2、SDK使用方法
在项目中引入jar包后即可使用。
```
# 以下为gradle的配置方式
implementation 'io.github.supperai:parse_sdk:1.0.5'

# 以下为maven的配置方式
<dependency>
    <groupId>io.github.supperai</groupId>
    <artifactId>parse_sdk</artifactId>
    <version>1.0.5</version>
</dependency>
```

# 3、SDK使用示例

本示例展示了如何使用 TextInParseX SDK 来解析 PDF 文件并提取其中的各种元素。
完整示例代码请见TextInParseX/src/test/TestSDK.java。

## 1. 初始化

首先，导入必要的包并初始化 `ParseXClient`：

```java
import com.textinparsex.ParseXClient;
import com.textinparsex.model.*;
import org.opencv.core.Mat;

import java.util.List;

public class TextInParseXExample {
    public static void main(String[] args) throws Exception {
        // 加载 OpenCV 库
        System.load("/path/to/opencv/library.so");

        // 初始化 ParseXClient
        ParseXClient parseXClient = new ParseXClient("your_app_id_here", "your_secret_code_here");

        // 指定要分析的 PDF 文件路径
        String pdfFilePath = "/path/to/your/document.pdf";

        // 指定解析的api url，请参考api文档确定参数
        String apiUrl = "https://api.textin.com/ai/service/v1/pdf_to_markdown?markdown_details=1&apply_document_tree=1&page_details=1&get_image=both";

        // 开始分析文档
        Document result = parseXClient.beginAnalyzeDocumentFromUrl(pdfFilePath, apiUrl);

        // ... 后续代码
    }
}
```

## 2. 获取 Markdown 内容

提取并打印文档的 Markdown 格式内容：

```java
System.out.println("Markdown content:");
System.out.println(result.getAllMarkdown());
```

## 3. 提取所有文本

获取并打印文档中的所有文本内容：

```java
System.out.println("\nAll text in document:");
parseXClient.printAllElements(result.getAllText(), 0, 1000);
```

## 4. 处理表格

获取并打印文档中的所有表格：

```java
System.out.println("\nTotal tables in document:");
List<Table> tables = result.getAllTables();
for (int i = 0; i < tables.size(); i++) {
    System.out.println("Table " + (i + 1) + ":");
    parseXClient.printAllElements(tables.get(i));
}
```

## 5. 处理段落和文本行

获取并打印文档中的所有段落和文本行：

```java
System.out.println("\nTotal paragraphs in document:");
List<Paragraph> paragraphs = result.getAllParagraphs();
for (int pIdx = 0; pIdx < paragraphs.size(); pIdx++) {
    Paragraph paragraph = paragraphs.get(pIdx);
    System.out.println("\n--- Paragraph " + (pIdx + 1) + "/" + paragraphs.size() + " ---");
    System.out.println("Paragraph position: " + paragraph.getPos());

    List<Object> lines = paragraph.getLines();
    for (int lIdx = 0; lIdx < lines.size(); lIdx++) {
        Object line = lines.get(lIdx);
        System.out.println("  Line " + (lIdx + 1) + "/" + lines.size());
        if (line instanceof ContentTextLine) {
            ContentTextLine textLine = (ContentTextLine) line;
            System.out.println("    Line position: " + textLine.getPos());
            System.out.println("    Line text: " + textLine.getText());
        }
    }
}
```

## 6. 处理图片

获取并打印文档中的所有图片信息：

```java
System.out.println("\nTotal images in document:");
List<ContentImage> images = result.getAllImages();
for (int i = 0; i < images.size(); i++) {
    System.out.println("Image " + (i + 1) + ":");
    parseXClient.printAllElements(images.get(i));
}
```

## 7. 处理 OpenCV Mat 图片

获取并打印文档中所有图片的 OpenCV Mat 对象：

```java
System.out.println("\nTotal images (as OpenCV Mat) in document:");
List<Mat> cvImages = result.getAllImagesCvMat();
for (int i = 0; i < cvImages.size(); i++) {
    System.out.println("Image " + (i + 1) + " (as OpenCV Mat):");
    parseXClient.printAllElements(cvImages.get(i));
}
```

## 8. 处理和保存带注释的图像

以下示例展示了如何处理文档中的每一页，为表格、图像、段落和文本行添加边界框，并保存结果图像：

```java
public void processAndSaveImages() {
    String downloadImageUrl = "https://api.textin.com/ocr_image/download?";

    for (Page page : priDocument.getPages()) {
        Mat pageImg = downloadImageFromUrl(downloadImageUrl, page.getImageId());
        if (pageImg == null) continue;

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
}
```

这个方法会为每个页面下载图像，然后在图像上绘制矩形来标注表格单元格（红色）、图像（黄色）、段落（绿色）和文本行（蓝色）。处理后的图像会以 "image_result_[页码].jpg" 的格式保存。

## 9. 处理表格并保存为Excel

```java
List<Table> tableList = result.getAllTables();
parseXClient.saveTablesAsExcel(tableList, "/your/path/to/example.xlsx");
```
这个方法会将表格转换为excel文件，并保存到指定路径。如果有多个表格，会生成多个sheet。


## 注意事项

使用此示例时，请确保：

1. 替换 `your_app_id_here` 和 `your_secret_code_here` 为您的实际 API ID 和密钥。
2. 更新 `System.load()` 中的路径，指向正确的 OpenCV 库文件。如果有完整opencv环境，可以使用System.loadLibrary(Core.NATIVE_LIBRARY_NAME)。
3. 将 `/path/to/your/document.pdf` 替换为要分析的 PDF 文件的实际路径。

这个示例展示了如何使用 TextInParseX SDK 的主要功能，包括提取 Markdown 内容、文本、表格、段落、图片信息等。您可以根据需要修改这个示例，以适应您的具体使用场景。

## 如果没有OpenCV环境
下载页面 https://github.com/opencv/opencv/releases。
建议下载版本为4.5.5

### 如果是windows系统，请安装opencv的windows版本。
下载opencv-455-vc14_vc15.exe，并安装。

安装完成后，找到opencv_java455.dll，System.load()中填入该文件的路径。

如果配置了完整的opencv环境，可以使用System.loadLibrary("opencv_java455")。

### 如果是linux系统，请下载source包并编译。
1. 下载并解压source包。

2. 确认是否安装了ant，通过ant -version确认。如果未安装，先执行 apt-get install ant 进行安装。

3. 进入解压后的目录，执行以下命令：
```
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D BUILD_TESTS=OFF -D BUILD_SHARED_LIBS=OFF -D BUILD_JAVA=ON ..
make -j8
```
cmake这一步需要确认配置是否正确，如果java下的ant不能为空，Java wrapper需要为YES。
<img src="image.png" alt="OpenCV configuration" width="80%" />

如果确认已经安装了ant，但cmake时仍然报错，请尝试以下命令：
```
ll /usr/bin/ant
# 如果显示为空或链接到相对路径，则执行以下命令：
sudo ln -s /usr/share/ant/bin/ant /usr/bin/ant
```
然后重新执行cmake命令。

4. 编译完成后，将生成的build/lib/opencv_java455.so文件复制到项目中，并在System.load()中填入该文件的路径。

## 如果有OpenCV环境但版本不匹配？
1. 参考上面安装步骤，重新安装指定版本的opencv。

2. 如果不想重新安装，可以修改gradle或maven中配置的org.openpnp的opencv版本以适配当前环境的opencv版本。
