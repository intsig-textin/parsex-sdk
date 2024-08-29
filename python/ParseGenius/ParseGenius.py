import cv2
import numpy as np
import base64
import json
from typing import List, Union, Optional
from pathlib import Path

# 定义用于表示 OpenAPI 架构组件的数据类
class TextBlock:
    def __init__(self, type: str, pos: List[int], content: List[int], sub_type: Optional[str] = None, is_continue: Optional[bool] = False):
        self.type = type
        self.pos = pos
        self.content = content
        self.sub_type = sub_type
        self.is_continue = is_continue

class ImageBlock:
    def __init__(self, type: str, pos: List[int], zorder: int, content: List[int]):
        self.type = type
        self.pos = pos
        self.zorder = zorder
        self.content = content

class TableCell:
    def __init__(self, row: int, col: int, pos: List[int], content: List[Union[TextBlock, ImageBlock]], row_span: Optional[int] = 1, col_span: Optional[int] = 1):
        self.row = row
        self.col = col
        self.pos = pos
        self.content = content
        self.row_span = row_span
        self.col_span = col_span

class Table:
    def __init__(self, type: str, pos: List[int], rows: int, cols: int,
                 columns_width: List[int], rows_height: List[int],
                 cells: List[TableCell], sub_type: Optional[str] = 'bordered',
                 is_continue: Optional[bool] = False, parse_type=None):
        self.type = type
        self.pos = pos
        self.rows = rows
        self.cols = cols
        self.columns_width = columns_width
        self.rows_height = rows_height
        self.cells = cells
        self.sub_type = sub_type
        self.is_continue = is_continue
        self.parse_type = parse_type  # 新增的字段


class Paragraph:
    def __init__(self, pos, content):
        self.pos = pos
        self.lines = content

class ImageData:
    def __init__(self, base64: Optional[str] = None, region: Optional[List[int]] = None, path: Optional[str] = None):
        self.base64 = base64
        self.region = region
        self.path = path

    def to_cv_mat(self) -> np.ndarray:
        if self.base64:
            image_data = np.frombuffer(base64.b64decode(self.base64), dtype=np.uint8)
            image = cv2.imdecode(image_data, cv2.IMREAD_COLOR)
            if image is None:
                raise ValueError("Cannot decode image from base64")
            return image
        else:
            raise ValueError("No valid image data found.")

class ContentImage:
    def __init__(self, id: int, type: str, pos: List[int], data: ImageData, sub_type: Optional[str] = None, stamp_type: Optional[str] = None, stamp_shape: Optional[str] = None, stamp_color: Optional[str] = None, size: Optional[List[int]] = None):
        self.id = id
        self.type = type
        self.pos = pos
        self.data = data
        self.sub_type = sub_type
        self.stamp_type = stamp_type
        self.stamp_shape = stamp_shape
        self.stamp_color = stamp_color
        self.size = size

class ContentTextLine:
    def __init__(self, id: int, type: str, text: str, pos: List[int], angle: Optional[int] = 0, sub_type: Optional[str] = None, direction: Optional[int] = 0, score: Optional[float] = None, char_pos: Optional[List[List[int]]] = None, char_cand: Optional[List[List[str]]] = None, char_cand_score: Optional[List[List[float]]] = None):
        self.id = id
        self.type = type
        self.text = text
        self.pos = pos
        self.angle = angle
        self.sub_type = sub_type
        self.direction = direction
        self.score = score
        self.char_pos = char_pos or []
        self.char_cand = char_cand or []
        self.char_cand_score = char_cand_score or []

class Page:
    def __init__(self, status: str, page_id: int, durations: float, image_id: Optional[str] = None, width: Optional[int] = None, height: Optional[int] = None, angle: Optional[int] = 0, num: Optional[int] = 0, image: Optional[ImageData] = None, content: Optional[List[Union[ContentTextLine, ContentImage]]] = None, structured: Optional[List[Union[Table]]] = None, structured_para: Optional[List[Union[Paragraph]]] = None):
        self.status = status
        self.page_id = page_id
        self.durations = durations
        self.image_id = image_id
        self.width = width
        self.height = height
        self.angle = angle
        self.num = num
        self.image = image
        self.content = content or []
        self.structured = structured or []
        self.structured_para = structured_para or []

    def get_tables(self) -> List[Table]:
        content_map = {c.id: c for c in self.content}
        new_tables = []
        for table in self.structured:
            if isinstance(table, dict):
                table = Table(**table)

            if isinstance(table, Table):
                new_cells = []
                for cell in table.cells:
                    if isinstance(cell, dict):
                        cell = TableCell(**cell)

                    new_content = []
                    for item in cell.content:
                        if isinstance(item, dict):
                            if item.get('type') == 'textblock':
                                item = TextBlock(**item)
                            elif item.get('type') == 'imageblock':
                                item = ImageBlock(**item)

                        if isinstance(item, TextBlock):
                            resolved_content = [content_map[content_id] for content_id in item.content]
                            new_content.append(TextBlock(
                                type=item.type,
                                pos=item.pos,
                                content=resolved_content,
                                sub_type=item.sub_type,
                                is_continue=item.is_continue
                            ))

                        elif isinstance(item, ImageBlock):
                            resolved_content = [content_map[content_id] for content_id in item.content]
                            new_content.append(ImageBlock(
                                type=item.type,
                                pos=item.pos,
                                zorder=item.zorder,
                                content=resolved_content
                            ))

                    new_cell = TableCell(
                        row=cell.row,
                        col=cell.col,
                        pos=cell.pos,
                        content=new_content,
                        row_span=cell.row_span,
                        col_span=cell.col_span
                    )
                    new_cells.append(new_cell)

                new_table = Table(
                    type=table.type,
                    pos=table.pos,
                    rows=table.rows,
                    cols=table.cols,
                    columns_width=table.columns_width,
                    rows_height=table.rows_height,
                    cells=new_cells,
                    sub_type=table.sub_type,
                    is_continue=table.is_continue
                )
                new_tables.append(new_table)
        return new_tables

    def get_paragraphs(self) -> List[Paragraph]:
        content_map = {c.id: c for c in self.content}
        paragraphs = []
        for item in self.structured_para:
            if isinstance(item, dict):
                resolved_content = [content_map[content_id] for content_id in item["content"]]
                para = Paragraph(item["pos"], resolved_content)
                paragraphs.append(para)
        return paragraphs

    def get_images(self) -> List[ContentImage]:
        return [item for item in self.content if isinstance(item, ContentImage)]

    def get_images_cv_mat(self) -> List[np.ndarray]:
        return [image.data.to_cv_mat() for image in self.get_images()]

    def get_text(self) -> str:
        return ''.join(item.text for item in self.content if isinstance(item, ContentTextLine))

class Metrics:
    def __init__(self, document_type: str, valid_page_number: int, paragraph_number: int, character_number: int):
        self.document_type = document_type
        self.valid_page_number = valid_page_number
        self.paragraph_number = paragraph_number
        self.character_number = character_number

class Document:
    def __init__(self, version: str, duration: int, result: dict, metrics: Metrics):
        self.version = version
        self.duration = duration
        self.result = result
        self.metrics = metrics

    def get_all_images(self) -> List[ContentImage]:
        images = []
        for page in self.result['pages']:
            images.extend(page.get_images())
        return images

    def get_all_images_cv_mat(self) -> List[np.ndarray]:
        images = self.get_all_images()
        return [image.data.to_cv_mat() for image in images]

    def get_all_text(self) -> str:
        return ''.join(page.get_text() for page in self.result['pages'])

    def get_all_tables(self) -> List[Table]:
        all_tables = []
        for page in self.result['pages']:
            all_tables.extend(page.get_tables())
        return all_tables

    def get_all_paragraphs(self) -> List[Paragraph]:
        paragraphs = []
        for page in self.result['pages']:
            paragraphs.extend(page.get_paragraphs())
        return paragraphs

    def get_all_markdown(self) -> str:
        markdown_details = self.result['detail']
        all_markdown = ''.join(item["text"] for item in markdown_details)
        return all_markdown

class Pdf2MdParserEngine:
    def __init__(self):
        self.pri_document = Document(
            version="1.0.0",
            duration=0,
            result={},
            metrics=Metrics(
                document_type="unknown",
                valid_page_number=0,
                paragraph_number=0,
                character_number=0
            )
        )

    @staticmethod
    def create_parse_genius():
        return Pdf2MdParserEngine()

    def parse(self, json_path):
        json_file_path = Path(json_path)
        with json_file_path.open('r', encoding='utf-8') as f:
            try:
                data = json.load(f)
            except json.JSONDecodeError as e:
                print(f"JSON 解析错误: {e}")
                return -1
        print('start to prase_x_to_markdown_output')
        self.pri_document = parse_x_to_markdown_output(data)
        return 0

    def get_document(self):
        return self.pri_document

    def get_page_size(self):
        return len(self.pri_document.result['pages'])

    def get_all_markdown(self) -> str:
        return self.pri_document.get_all_markdown()

    def print_all_elements(self, obj, indent=0, max_str_length=50):
            indent_space = '  ' * indent

            def format_value(value):
                if isinstance(value, str) and len(value) > max_str_length:
                    return value[:max_str_length] + '...'
                return value

            if isinstance(obj, list):
                if obj and isinstance(obj[0], int):
                    print(f'{indent_space}{obj}')
                else:
                    for i, item in enumerate(obj):
                        print(f'{indent_space}[{i}]')
                        self.print_all_elements(item, indent + 1)
            elif isinstance(obj, dict):
                for key, value in obj.items():
                    print(f'{indent_space}{key}: ', end='')
                    if isinstance(value, (list, dict)):
                        if isinstance(value, list) and value and isinstance(value[0], int):
                            print(f'{value}')
                        else:
                            print()
                            self.print_all_elements(value, indent + 1)
                    else:
                        print(f'{format_value(value)}')
            elif hasattr(obj, '__dict__'):
                for key, value in vars(obj).items():
                    print(f'{indent_space}{key}: ', end='')
                    if isinstance(value, (list, dict)):
                        print()
                        self.print_all_elements(value, indent + 1)
                    else:
                        print(f'{format_value(value)}')
            else:
                print(f'{indent_space}{format_value(obj)}')


# 辅助函数用于解析 JSON 数据
def parse_x_to_markdown_output(data: dict) -> Document:
    result_data = data.get('result', {})
    metrics_data = data.get('metrics', {})

    # 处理 pages 数据
    pages = [
        Page(
            status=page.get('status', 'success'),
            page_id=page.get('page_id', 0),
            durations=page.get('durations', 0.0),
            image_id=page.get('image_id'),
            width=page.get('width'),
            height=page.get('height'),
            angle=page.get('angle', 0),
            num=page.get('num', 0),
            image=ImageData(**page.get('image', {})) if 'image' in page else None,
            content=[ContentTextLine(**c) if c.get('type') == 'line' else
                     ContentImage(id=c.get('id'), type=c.get('type'), pos=c.get('pos'),
                                  data=ImageData(**c['data']), sub_type=c.get('sub_type'),
                                  stamp_type=c.get('stamp_type'), stamp_shape=c.get('stamp_shape'),
                                  stamp_color=c.get('stamp_color'), size=c.get('size'))
                     for c in page.get('content', [])],
            structured=[
                Table(**{k: v for k, v in s.items() if k != 'parse_type'})
                for s in page.get('structured', [])
                if s.get('type') == 'table'
            ],
            structured_para=[s for s in page.get('structured', []) if s.get('type') == 'textblock']
        )
        for page in result_data.get('pages', [])
    ]

    # 假设 metrics_data 是一个字典
    if isinstance(metrics_data, dict):
        metrics = Metrics(
            document_type=metrics_data.get('document_type', 'unknown'),
            valid_page_number=metrics_data.get('valid_page_number', 0),
            paragraph_number=metrics_data.get('paragraph_number', 0),
            character_number=metrics_data.get('character_number', 0)
        )
    else:
        # 如果 metrics_data 是一个列表或其他结构，处理方法会有所不同
        metrics = Metrics(
            document_type='unknown',
            valid_page_number=0,
            paragraph_number=0,
            character_number=0
        )

    return Document(
        version=data.get('version', ''),
        duration=data.get('duration', 0),
        result={"pages": pages},  # 确保 `result` 中的 `pages` 是 `Page` 对象列表
        metrics=metrics
    )



class CodeAndMessage:
    def __init__(self, code: int, message: str):
        self.code = code
        self.message = message

def parse_code_and_message(data: dict) -> CodeAndMessage:
    return CodeAndMessage(
        code=data.get('code', 200),
        message=data.get('message', 'success')
    )
