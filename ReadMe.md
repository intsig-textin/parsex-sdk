# 整体介绍
ParseGenius是一套标准的多平台支持的python sdk，帮助开发者解析[pdf_to_markdown](https://www.textin.com/document/pdf_to_markdown)Restful API返回结果，获取对应的版面元素的数据结构。开发者只需`pip install ParseGenius`安装对应的依赖就可以使用。

# 快速开始

### 基本的使用方式
引用依赖：
```python
import ParseGenius
```
初始化解析器：
```python
# 初始化解析器
parser = Pdf2MdParserEngine.create_parse_genius()
# json文件路径
json_path = 'example.json'
# 调用解析方法
parser.parse(json_path)
# 获取页数（为了方便访问页元素，请先获取页总数）
total_pages = parser.get_page_size()
```
获取表格：
```python
# 遍历每一页
for page_id in range(total_pages):
	# 获取当前页面的表格数组
    tables = parser.find_tables(page_id)
    # 遍历每一个表格
    for index, table in enumerate(tables):
    	# 使用表格对象table，这里展示输出每个table版面元素
    	parser.print_all_elements(table)
```
获取段落：
```python
# 遍历每一页
for page_id in range(total_pages):
	# 获取当前页面的段落
    paragraphs = parser.get_paragraph(page_id)
    # 遍历每一个段落
    for p_idx, each_paragraph in enumerate(paragraphs):
    	# 打印每个段落的位置
    	print(f"Paragraph position: {each_paragraph.pos}")
```
获取图片：
```python
# 遍历每一页
for page_id in range(total_pages):
	# 获取当前页面的图片数组
    images = parser.get_images(page_id)
    # 循环使用每个图片对象
   	for index, image in enumerate(images):
    	parser.print_all_elements(image)
```

# Lower-Level访问
通过使用`ParseGenius`解析json，开发者可以通过多层次的类结构访问版面元素，以下是版面元素的类结构解释。

### 1. TextBlock
表示表格单元格中的文本块。

#### 属性:
- `type` (str): 文本块的类型。
- `pos` (List[int]): 文本块的位置，通常表示为坐标或索引。
- `content` (List[int]): 文本块的内容索引。
- `sub_type` (Optional[str], 可选): 文本块的子类型，默认为 `None`。
- `is_continue` (Optional[bool], 可选): 指示文本块是否与上一个文本块连续，默认为 `False`。

### 2. ImageBlock
表示表格单元格中的图像块。

#### 属性:
- `type` (str): 图像块的类型。
- `pos` (List[int]): 图像块的位置。
- `zorder` (int): 图像块的绘制顺序，较高的值表示图像绘制在前。
- `content` (List[int]): 图像块的内容索引。

### 3. TableCell
表示表格中的一个单元格，可能包含文本块或图像块。

#### 属性:
- `row` (int): 单元格所在的行号。
- `col` (int): 单元格所在的列号。
- `pos` (List[int]): 单元格的位置。
- `content` (List[Union[TextBlock, ImageBlock]]): 单元格的内容，可以是文本块或图像块的列表。
- `row_span` (Optional[int], 可选): 单元格占据的行数，默认为 `1`。
- `col_span` (Optional[int], 可选): 单元格占据的列数，默认为 `1`。

### 4. Table
表示一个结构化的表格。

#### 属性:
- `type` (str): 表格的类型。
- `pos` (List[int]): 表格的位置。
- `rows` (int): 表格的行数。
- `cols` (int): 表格的列数。
- `columns_width` (List[int]): 每列的宽度。
- `rows_height` (List[int]): 每行的高度。
- `cells` (List[TableCell]): 表格中的所有单元格。
- `sub_type` (Optional[str], 可选): 表格的子类型，默认为 `bordered`。
- `is_continue` (Optional[bool], 可选): 表示表格是否连续，默认为 `False`。

### 5. Paragraph
表示一个结构化的段落。

#### 属性:
- `pos`: 段落的位置。
- `lines`: 段落中的内容，通常是文本行的集合。

### 6. ImageData
表示页面图像数据。

#### 属性:
- `base64` (Optional[str], 可选): 图像的 Base64 编码字符串。
- `region` (Optional[List[int]], 可选): 图像所在的区域。
- `path` (Optional[str], 可选): 图像文件的路径。


### 7. ContentImage
表示页面中的一个图像元素内容，其他的版面元素可以通过索引找到指定的ContentImage。

#### 属性:
- `id` (int): 图像的唯一标识符。
- `type` (str): 图像的类型。
- `pos` (List[int]): 图像的位置。
- `data` (ImageData): 图像的具体数据。
- `sub_type` (Optional[str], 可选): 图像的子类型，默认为 `None`。
- `stamp_type` (Optional[str], 可选): 图章类型（如果图像是图章）。
- `stamp_shape` (Optional[str], 可选): 图章的形状。
- `stamp_color` (Optional[str], 可选): 图章的颜色。
- `size` (Optional[List[int]], 可选): 图像的大小。

### 8. ContentTextLine
表示页面中的一行文本元素内容。其他的版面元素可以通过索引找到指定的ContentTextLine。

#### 属性:
- `id` (int): 文本行的唯一标识符。
- `type` (str): 文本行的类型。
- `text` (str): 文本内容。
- `pos` (List[int]): 文本行的位置。
- `angle` (Optional[int], 可选): 文本的旋转角度，默认为 `0`。
- `sub_type` (Optional[str], 可选): 文本行的子类型，默认为 `None`。
- `direction` (Optional[int], 可选): 文本行的方向，默认为 `0`。
- `score` (Optional[float], 可选): 识别的可信度得分。
- `char_pos` (Optional[List[List[int]]], 可选): 字符的位置列表。
- `char_cand` (Optional[List[List[str]]], 可选): 字符的候选列表。
- `char_cand_score` (Optional[List[List[float]]], 可选): 字符候选的得分列表。

### 9. Page
表示页面的整体结构，是访问页面元素的入口点，包含多种内容元素。

#### 属性:
- `status` (str): 页面状态。
- `page_id` (int): 页面的唯一标识符。
- `durations` (float): 页面处理的时长。
- `image_id` (Optional[str], 可选): 与页面关联的图像的唯一标识符。
- `width` (Optional[int], 可选): 页面宽度。
- `height` (Optional[int], 可选): 页面高度。
- `angle` (Optional[int], 可选): 页面的旋转角度，默认为 `0`。
- `num` (Optional[int], 可选): 页面的编号，默认为 `0`。
- `image` (Optional[ImageData], 可选): 页面的图像数据。
- `content` (Optional[List[Union[ContentTextLine, ContentImage]]], 可选): 页面的内容列表，包含文本行和图像。
- `structured` (Optional[List[Union[Table]]], 可选): 页面的结构化表格列表。
- `structured_para` (Optional[List[Union[Paragraph]]], 可选): 页面的结构化段落列表。



# High-Level访问
通过使用`ParseGenius`解析json，开发者可以使用API访问版面元素，相比于`Lower-Level`的方式，API提供了更强大的封装能力以及定制化访问策略。


### 1. `find_tables(page_id: int)`
获取指定页面中的所有表格。

#### 参数:
- `page_id` (int): 需要检索的页面 ID。

#### 返回值:
- `List[Table]`: 返回一个page_id对应页的包含所有表格对象的列表。

#### 说明:
- 该方法首先通过 `page_id` 在文档中找到对应的页面。
- 然后，它解析页面中的结构化表格，逐个处理每个单元格及其内容（文本块或图像块）。
- 最终返回一个包含表格的列表，每个表格对象包含完整的表格结构信息，也就是通过Content还原之后的表格。


### 2. `get_paragraph(page_id: int)`
获取指定页面中的所有段落。

#### 参数:
- `page_id` (int): 需要检索的页面 ID。

#### 返回值:
- `List[Paragraph]`: 返回一个包含所有段落对象的列表。

#### 说明:
- 该方法通过 `page_id` 找到文档中对应的页面。
- 然后解析页面中的结构化段落，逐个处理段落中的内容（如文本行）。
- 最终返回一个段落列表，每个段落对象包含段落的位置和内容。


### 3. `get_images(page_id: int)`
获取指定页面中的所有图像。

#### 参数:
- `page_id` (int): 需要检索的页面 ID。

#### 返回值:
- `List[ContentImage]`: 返回一个包含所有图像对象的列表。

#### 说明:
- 该方法通过 `page_id` 找到文档中对应的页面。
- 然后从页面内容中筛选出所有图像元素，并将它们添加到返回的列表中。


### 4. `get_text(page_id: int)`
获取指定页面中的所有文本，并将其合并为一个字符串。

#### 参数:
- `page_id` (int): 需要检索的页面 ID。

#### 返回值:
- `str`: 返回一个包含所有文本行的合并字符串。

#### 说明:
- 该方法通过 `page_id` 找到文档中对应的页面。
- 然后提取页面中的所有文本行，并将它们合并为一个完整的字符串返回。


### 5. `get_markdown(page_id: int)`
获取指定页面的 Markdown 格式内容。

#### 参数:
- `page_id` (int): 需要检索的页面 ID。

#### 返回值:
- `str`: 返回指定页面的 Markdown 格式内容。

#### 说明:
- 该方法从文档结果中的详情部分提取出与指定 `page_id` 相关的 Markdown 内容，并将这些内容合并为一个完整的字符串返回。

### 6. `get_document()`
获取原始文档对象。

#### 返回值:
- 返回原始的文档对象 `pri_document`，包含所有页面和内容的结构化信息。

#### 说明:
- 该方法用于获取整个文档的原始数据结构，便于进一步的操作或分析。

### total get_all*/find_all*
所有带有以上前缀的API都为获取整个document的版面元素集合

#### 说明:
- 区别于需要`page_id`参数，可以直接调用获取整个版面的对应元素结合。
- 目前支持`find_all_tables`/`get_all_images`/`get_all_text`/`get_all_paragraphs`/`get_all_markdown`

