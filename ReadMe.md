# 整体介绍
x2md_tools 提供了一套sdk， 帮助开发者解析[pdf_to_markdown](https://www.textin.com/document/x_to_markdown)的结果，获取对应的版面元素的数据结构


## 快速开始

### 基本的使用方式

#### Linux SDK
处理表格数据：
```cpp
ITextInParserEngine* engine = ITextInParserEngine::createAndStartTextInParserEngine();
engine->parseFile(filePath.c_str()); // filePath为需要解析的json文件路径
int pageSize = 0;
engine->getPageSize(&pageSize)
for (int page_i = 0; page_i < pageSize; ++page_i) {
  int tableSize = 0;
  engine->getPageTablesSize(page_i, &tableSize);
  std::vector<Table> tables(tableSize);
  engine->findTables(page_i, tables.data(), tableSize);
  for (const auto& table : tables) {
    print_table_info(table); // 可以看到每一个表格的数据信息
  }
}
engine->release();
delete engine;
```
#### Python
处理表格数据：
```python
# 初始化解析器
parser = SimpleTextInParserEngine.createAndStartTextInParserEngine()
# 测试文件路径
test_json_path = 'chinese-tables.json'
# 调用解析方法
parser.parse(test_json_path)
# 检查解析结果是否为预期
self.assertIsInstance(parser.pri_document, XToMarkdownOutput)
# 获取当前的页数
total_pages = parser.getPageSize()
print(f"Total valid page size: {total_pages}")
# 遍历每一页
for page_id in range(total_pages):
    print(f"=== Page {page_id + 1} ===")
    print("\n")
    # 获取当前页面的表格数组
    tables = parser.findTables(page_id)
    # 循环打印每个表格对象
    for index, table in enumerate(tables):
        print(f"Table {index + 1}:")
        parser.print_all_elements(table)
        print("\n")
    # 获取当前页面的图片数组
    images = parser.findImages(page_id)
    # 循环打印每个图片对象
    for index, image in enumerate(images):
        print(f"Image {index + 1}:")
        parser.print_all_elements(image) # 限定只能打印前50(默认)个字符
        print("\n")
    # 获取当前页面的文本信息
    text = parser.findText(page_id)
    print("Text:")
    parser.print_all_elements(text, 0, 1000) # 限定只能打印前1000个字符
    print("\n")
```
#### Python SDK
处理表格数据：
```python
engine = textin_parser.ITextInParserEngine.createAndStartTextInParserEngine()
engine.parseFile("file.json")
result, page_size = engine.getPageSize()
if page_size > 0:
    # 获取第一页的表格数量
    result, table_size = engine.getPageTablesSize(0)
    # 获取第一页的表格数组
    result, tables = engine.findTables(0, table_size)
        for i, table in enumerate(tables):
            print_table(table)
```

# Linux SDK介绍

## Linux SDK版本相关
| 版本         | 说明                                     |
|------------|----------------------------------------|
| 0.0.1（当前版本）      | 支持提取json中`table`数据与`cell`数据，支持Linux C版本与Python版本      |

## 代码框架
- `linux_sdk/include`目录包含所需调用所用到的头文件
- `linux_sdk/samples`目录包含获取表格信息的演示用程序代码与pdf/json文件
- `linux_sdk/src`目录包含功能实现的具体源文件
- `linux_sdk/third_party`目录包含编译所需要的第三方代码文件

## 编译运行
- 环境准备: gcc/g++ 8.1及以上
- sample编译：到`samples/cpp`目录下，直接运行`./build.sh`即可编译出测试程序
- sample运行会遍历`resource/`的json文件，sample会处理这些json文件，结果都输出在`output/log.txt`中
- 每轮执行都会创建新的`output/log.txt`

## 运行样例
- 项目clone下来之后，可以进入`samples/cpp`目录，运行`./test_program`
- 在`output/log.txt`中即可看到`samples/resource`内的演示表格输出

## 使用方式


### SDK API使用说明

- `createAndStartTextInParserEngine`获取一个Parser的实列
- `parse` 解析json的原始字符串
- `parseFile` 根据json文件路径读取json数据解析
- `release` 释放Parser内部的资源
- `getPageSize` 获取PDF中的Page数量
- `getPageTablesSize` 获取具体Page的表格数量
- `findTables` 获取具体Page的表格
- `getDocument` 获取整个PDF数据的结构体

## Sample代码框架
### 说明
- `samples/base`目录包含对应的工具函数文件
- `samples/cpp`目录包含演示的main函数以及可直接运行的`Makefile`与`build.sh`编译脚本
- `samples/file`目录包含演示用的pdf文件
- `samples/resource`目录包含file文件中通过TEXTIN RESTAPI返回的json结果并保存成了json文件（为了方便演示，直接展示处理`samples/resource`目录里json）
- output目录包含输出的结果文件log.txt

# Python介绍
- `python_sdk`是对Cpp中接口通过Pybind11做的二次封装。所以和Linux SDK提供的C++ API功能完全一样，每次需要运行`build_so.sh`脚本构建指定`python3`版本的so
- `python`是由纯python语言编写的pip包，可以在任何操作系统上运行，目前已上传到pip官网，可以直接使用`pip install text_in_parser_engine`加载使用

## Python SDK版本
| 版本         | 说明                                     |
|------------|----------------------------------------|
| 0.0.1（当前版本）      | 支持提取json中`table`数据与`cell`数据      |


## Python PiP包版本
| 版本         | 说明                                     |
|------------|----------------------------------------|
| 0.0.3（当前版本）      | 支持提取json中`table`数据与`cell`数据，支持提取页中的图片以及文本数据    |


## Python PiP数据类说明
通过将json字符串直接还原成python语言的数据类结构，在原始的open api返回结果上不做任何修改

XToMarkdownOutput
- version (str): 文档解析引擎的版本号。
- duration (int): 文档解析引擎处理文档所需的时间（以毫秒为单位）。
- result (dict): 包含文档的解析结果，通常包括解析后的 Markdown 内容和页面信息。
- metrics (Metrics): 包含关于文档的各种统计信息的对象。
  
Metrics
- document_type (str): 文档类型（例如：pdf, word, excel 等）。
- total_page_number (int): 文档的总页数。
- valid_page_number (int): 成功解析的有效页数。
- paragraph_number (int): 文档中的段落数。
- character_number (int): 文档中的字符总数。
  
PriPage
- status (str): 当前页面的解析状态
- page_id (int): 页面编号。
- durations (float): 处理当前页面所需的时间（以毫秒为单位）。
- image_id (Optional[str]): 页面图像的唯一标识符（可选）。
- width (Optional[int]): 页面宽度（可选）。
- height (Optional[int]): 页面高度（可选）。
- angle (Optional[int]): 页面旋转角度（可选）。
- num (Optional[int]): 页面编号（可选）。
- image (Optional[PriPageImageData]): 页面图像数据。
- content (Optional[List[Union[PriPageContentTextLine, PriPageContentImage]]]): 页面内的内容，包括文本行和图像。
- structured (Optional[List[Union[PriPageStructuredTable]]]): 页面内的结构化数据，如表格。
  
PriPageStructuredTable
- type (str): 表格类型（通常为 "table"）。
- pos (List[int]): 表格的坐标位置。
- rows (int): 表格的行数。
- cols (int): 表格的列数。
- columns_width (List[int]): 每列的宽度列表。
- rows_height (List[int]): 每行的高度列表。
- cells (List[PriPageStructuredTableTableCell]): 表格单元格的列表。
- sub_type (Optional[str]): 表格的子类型（默认 "bordered"）。
- is_continue (Optional[bool]): 表示表格是否在多页之间连续。
  
PriPageStructuredTableTableCell
- row (int): 单元格所在的行。
- col (int): 单元格所在的列。
- pos (List[int]): 单元格的坐标位置。
- content (List[Union[PriPageStructuredTableTableCellTextBlock, PriPageStructuredTableTableCellImageBlock]]]): 单元格的内容，可以是文本块或图像块。
- row_span (Optional[int]): 单元格的行跨度（默认为 1）。
- col_span (Optional[int]): 单元格的列跨度（默认为 1）。
  
PriPageStructuredTableTableCellTextBlock
- type (str): 内容类型（通常为 "textblock"）。
- pos (List[int]): 文本块的坐标位置。
- content (List[int]): 与此文本块关联的文本行 ID 列表。
- sub_type (Optional[str]): 文本块的子类型（可选）。
- is_continue (Optional[bool]): 表示文本块是否在多页之间连续。
  
PriPageStructuredTableTableCellImageBlock
- type (str): 内容类型（通常为 "imageblock"）。
- pos (List[int]): 图像块的坐标位置。
- zorder (int): 图像块的 Z 轴顺序。
- content (List[int]): 与此图像块关联的图像 ID 列表。
  
PriPageContentTextLine
- id (int): 文本行的唯一标识符。
- type (str): 内容类型（通常为 "line"）。
- text (str): 文本行的内容。
- pos (List[int]): 文本行的坐标位置。
- angle (Optional[int]): 文本行的旋转角度（可选）。
- sub_type (Optional[str]): 文本行的子类型（可选）。
- direction (Optional[int]): 文本行的方向（可选）。
- score (Optional[float]): 文本行的置信度评分（可选）。
- char_pos (Optional[List[List[int]]]): 文本行中每个字符的坐标位置（可选）。
- char_cand (Optional[List[List[str]]]): 文本行中每个字符的候选列表（可选）。
- char_cand_score (Optional[List[List[float]]]): 文本行中每个字符的候选评分列表（可选）。
  
PriPageContentImage
- id (int): 图像的唯一标识符。
- type (str): 内容类型（通常为 "image"）。
- pos (List[int]): 图像的坐标位置。
- data (PriPageContentImageData): 图像的数据对象，包含图像的实际数据或路径。
- sub_type (Optional[str]): 图像的子类型（可选）。
- stamp_type (Optional[str]): 图像的印章类型（可选）。
- stamp_shape (Optional[str]): 图像的印章形状（可选）。
- stamp_color (Optional[str]): 图像的印章颜色（可选）。
- size (Optional[List[int]]): 图像的尺寸（可选）。


### Python PiP数据类关系树状图
```
XToMarkdownOutput
├── result (dict)
│   └── pages (List[PriPage])
│       ├── content (List[Union[TextLine, Image]])
│       │   ├── TextLine
│       │   └── Image
│       └── structured (List[Table])
│           ├── Table
│           │   ├── cells (List[Cell])
│           │   │   ├── Cell
│           │   │   │   ├── content (List[Union[TextBlock, ImageBlock]])
│           │   │   │   │   ├── TextBlock
│           │   │   │   │   └── ImageBlock
│           │   │   └── columns_width, rows_height
│           │   └── sub_type, is_continue
├── metrics (Metrics)
│   └── document_type, total_page_number, valid_page_number, paragraph_number, character_number
└── version, duration
```


## 测试输出
以下是python_sdk编译C++的.so的步骤
- 环境准备: gcc/g++ 8/9 python3为3.6及其以上
- 进入`python_sdk/`，运行`build_so.sh`即可在当前目录下获得与`python3`版本一致的so
- 生成得到so后使用`python3`运行`python_sdk/`下的`parser_test.py`，即可在`output.log`看到所展示的对当前目录下`chinese-tables.json`的解析结果
- 开发者可以使用`import textin_parser`来访问so里提供的接口，使用时需要把so加入到开发者目录中

以下是python pip包的使用步骤
- 使用`pip install text_in_parser_engine`直接安装text_in_parser模块
- 进入`python/`，运行`test_parser.py`可看到演示的输出内容，推荐将信息重定向到文件中`python3 test_parser.py > output.log 2>&1`


