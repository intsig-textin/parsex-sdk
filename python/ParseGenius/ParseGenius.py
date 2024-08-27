import json
from typing import List, Dict, Union, Optional
from pathlib import Path
# Define the data classes to represent the OpenAPI schema components

class TextBlock:
    def __init__(self, type: str, pos: List[int],
            content: List[int], sub_type: Optional[str] = None,
            is_continue: Optional[bool] = False):
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
    def __init__(self, row: int, col: int, pos: List[int],
            content: List[Union[TextBlock,
                                ImageBlock]],
            row_span: Optional[int] = 1,
            col_span: Optional[int] = 1):
        self.row = row
        self.col = col
        self.pos = pos
        self.content = content
        self.row_span = row_span
        self.col_span = col_span

class Table:
    def __init__(self, type: str, pos: List[int], rows: int, cols: int,
            columns_width: List[int], rows_height: List[int],
            cells: List[TableCell],
            sub_type: Optional[str] = 'bordered', is_continue: Optional[bool] = False):
        self.type = type
        self.pos = pos
        self.rows = rows
        self.cols = cols
        self.columns_width = columns_width
        self.rows_height = rows_height
        self.cells = cells
        self.sub_type = sub_type
        self.is_continue = is_continue

class Paragraph:
    def __init__(self, pos, content):
        self.pos = pos
        self.lines = content


class ImageData:
    def __init__(self, base64: Optional[str] = None, region: Optional[List[int]] = None,
            path: Optional[str] = None):
        self.base64 = base64
        self.region = region
        self.path = path

class ContentImage:
    def __init__(self, id: int, type: str, pos: List[int], data: ImageData,
                sub_type: Optional[str] = None,
                stamp_type: Optional[str] = None,
                stamp_shape: Optional[str] = None,
                stamp_color: Optional[str] = None,
                size: Optional[List[int]] = None):
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
    def __init__(self, id: int, type: str, text: str, pos: List[int],
                angle: Optional[int] = 0,
                sub_type: Optional[str] = None,
                direction: Optional[int] = 0,
                score: Optional[float] = None,
                char_pos: Optional[List[List[int]]] = None,
                char_cand: Optional[List[List[str]]] = None,
                char_cand_score: Optional[List[List[float]]] = None):
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
    def __init__(self, status: str, page_id: int, durations: float,
            image_id: Optional[str] = None, width: Optional[int] = None,
            height: Optional[int] = None, angle: Optional[int] = 0,
            num: Optional[int] = 0, image: Optional[ImageData] = None,
            # readable: Optional[List[Union[Paragraph, TableText]]] = None,
            content: Optional[List[Union[ContentTextLine, ContentImage]]] = None,
            structured: Optional[List[Union[Table]]] = None, structured_para:Optional[List[Union[Paragraph]]] = None):
        self.status = status
        self.page_id = page_id
        self.durations = durations
        self.image_id = image_id
        self.width = width
        self.height = height
        self.angle = angle
        self.num = num
        self.image = image
        # self.readable = readable or []
        self.content = content or []
        self.structured = structured or []
        self.structured_para = structured_para or []

class Metrics:
    def __init__(self, document_type: str,
            valid_page_number: int, paragraph_number: int, character_number: int):
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

class CodeAndMessage:
    def __init__(self, code: int, message: str):
        self.code = code
        self.message = message

# Example of how to parse the response JSON into these structures

def parse_x_to_markdown_output(data: dict) -> Document:
    result_data = data.get('result', {})
    metrics_data = data.get('metrics', {})

    def filter_table_data(s):
        allowed_keys = {'type', 'pos', 'rows', 'cols',
                        'columns_width', 'rows_height',
                        'cells', 'sub_type', 'is_continue'}
        return {key: s[key] for key in allowed_keys if key in s}

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
            image=ImageData(**page.get('image', {})),
            content=[ContentTextLine(**c) if c.get('type') == 'line' else
                     ContentImage(**c)
                     for c in page.get('content', [])],
            structured=[
                Table(**filter_table_data(s))
                for s in page.get('structured', [])
                if s.get('type') == 'table'
            ],
            structured_para = [s for s in page.get('structured', []) if s.get('type') == 'textblock']
        )
        for page in result_data.get('pages', [])
    ]
    #for page in pages:
        #print(page.structured)
        #exit(0)
    detail = result_data['detail']
    result = {
        'src_page_count': result_data.get('src_page_count', 0),
        'markdown': result_data.get('markdown', ''),
        'pages': pages,
        'detail': detail
    }

    metrics = Metrics(
       document_type=result_data.get('document_type', 'unknown'),
       valid_page_number=result_data.get('valid_page_number', 0),
       paragraph_number=result_data.get('paragraph_number', 0),
       character_number=result_data.get('character_number', 0)
    )

    return Document(
        version=data.get('version', ''),
        duration=data.get('duration', 0),
        result=result,
        metrics=metrics
    )

def parse_code_and_message(data: dict) -> CodeAndMessage:
    return CodeAndMessage(
        code=data.get('code', 200),
        message=data.get('message', 'success')
    )

class Pdf2MdParserEngine:
    def __init__(self):
        # 提供默认值给 Document 的构造函数
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
                return -1  # 错误返回 -1
        print('start to prase_x_to_markdown_output')
        self.pri_document = parse_x_to_markdown_output(data)
        # self.print_all_elements(self.pri_document)
        return 0

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

    def find_tables(self,page_id: int) -> List[Table]:
        page = next((p for p in self.pri_document.result['pages'] if p.page_id == page_id), None)

        if not page:
            raise ValueError(f"Page with page_id {page_id} not found.")

        content_map = {c.id: c for c in page.content}

        new_tables = []

        for table in page.structured:
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

    def get_paragraph(self,page_id: int) -> List[Paragraph]:
        page = next((p for p in self.pri_document.result['pages'] if p.page_id == page_id), None)

        if not page:
            raise ValueError(f"Page with page_id {page_id} not found.")

        content_map = {c.id: c for c in page.content}

        paragraphs = []
        for item in page.structured_para:
            if isinstance(item, dict):
                resolved_content = [content_map[content_id] for content_id in item["content"]]
                para = Paragraph(item["pos"], resolved_content)
                paragraphs.append(para)

        return paragraphs

    def get_images(self, page_id: int) -> List[ContentImage]:
            page = next((p for p in self.pri_document.result['pages'] if p.page_id == page_id), None)

            if not page:
                raise ValueError(f"Page with page_id {page_id} not found.")

            images = [item for item in page.content if isinstance(item, ContentImage)]

            return images

    def get_text(self, page_id: int) -> str:
        page = next((p for p in self.pri_document.result['pages'] if p.page_id == page_id), None)

        if not page:
            raise ValueError(f"Page with page_id {page_id} not found.")

        combined_text = ''.join(item.text for item in page.content if isinstance(item, ContentTextLine))

        return combined_text

    def get_markdown(self, page_id: int) -> str:
        markdown_details = self.pri_document.result['detail']
        # for item in markdown_details:
        #     print(item)
        page_markdown = ''.join(item["text"] for item in markdown_details if item["page_id"] == page_id)
        return page_markdown


    def get_document(self):
        return self.pri_document

    def get_page_size(self):
        return len(self.pri_document.result['pages'])

    def find_all_tables(self) -> List[Table]:
        all_tables = []
        for page in self.pri_document.result['pages']:
            all_tables.extend(self.find_tables(page.page_id))
        return all_tables

    def get_all_images(self) -> List[ContentImage]:
        images = []
        for page in self.pri_document.result['pages']:
            images.extend(self.get_images(page.page_id))
        return images

    def get_all_text(self) -> str:
        all_text = ''
        for page in self.pri_document.result['pages']:
            all_text += self.get_text(page.page_id)
        return all_text

    def get_all_paragraphs(self) -> List[Paragraph]:
        paragraphs = []
        for page in self.pri_document.result['pages']:
            paragraphs.extend(self.get_paragraph(page.page_id))
        return paragraphs

    def get_all_markdown(self) -> str:
        markdown_details = self.pri_document.result['detail']
        all_markdown = ''.join(item["text"] for item in markdown_details)
        return all_markdown

