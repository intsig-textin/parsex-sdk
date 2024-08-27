# 整体介绍
本分支是专门为pdf_to_markdwon提供的一套sdk， 帮助开发者解析[pdf_to_markdown](https://www.textin.com/document/pdf_to_markdown)的结果，获取对应的版面元素的数据结构

## 快速开始

### 基本的使用方式

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

## 介绍
- `python`是由纯python语言编写的pip包，可以在任何操作系统上运行，目前已上传到pip官网，可以直接使用`pip install text_in_parser_engine`加载使用

## 测试输出
以下是python pip包的使用步骤
- 使用`pip install text_in_parser_engine`直接安装text_in_parser模块
- 进入`python/`，运行`test_parser.py`可看到演示的输出内容，推荐将信息重定向到文件中`python3 test_parser.py > output.log 2>&1`
- 输出是根据`file/test-2333.pdf`的版面元素来作为演示结果


