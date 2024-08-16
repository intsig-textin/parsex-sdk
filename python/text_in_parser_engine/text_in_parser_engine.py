import json
from typing import List, Dict, Union, Optional
from pathlib import Path
# Define the data classes to represent the OpenAPI schema components

class PriPageStructuredTableTableCellTextBlock:
    def __init__(self, type: str, pos: List[int],
            content: List[int], sub_type: Optional[str] = None,
            is_continue: Optional[bool] = False):
        self.type = type
        self.pos = pos
        self.content = content
        self.sub_type = sub_type
        self.is_continue = is_continue

class PriPageStructuredTableTableCellImageBlock:
    def __init__(self, type: str, pos: List[int], zorder: int, content: List[int]):
        self.type = type
        self.pos = pos
        self.zorder = zorder
        self.content = content

class PriPageStructuredTableTableCell:
    def __init__(self, row: int, col: int, pos: List[int],
            content: List[Union[PriPageStructuredTableTableCellTextBlock,
                                PriPageStructuredTableTableCellImageBlock]],
            row_span: Optional[int] = 1,
            col_span: Optional[int] = 1):
        self.row = row
        self.col = col
        self.pos = pos
        self.content = content
        self.row_span = row_span
        self.col_span = col_span

class PriPageStructuredTable:
    def __init__(self, type: str, pos: List[int], rows: int, cols: int,
            columns_width: List[int], rows_height: List[int],
            cells: List[PriPageStructuredTableTableCell],
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


class PriPageImageData:
    def __init__(self, base64: Optional[str] = None, region: Optional[List[int]] = None,
            path: Optional[str] = None):
        self.base64 = base64
        self.region = region
        self.path = path

class PriPageContentImageData:
    def __init__(self, base64: Optional[str] = None, region: Optional[List[int]] = None,
            path: Optional[str] = None):
        self.base64 = base64
        self.region = region
        self.path = path


class PriPageContentImage:
    def __init__(self, id: int, type: str, pos: List[int], data: PriPageContentImageData,
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

class PriPageContentTextLine:
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

class PriPage:
    def __init__(self, status: str, page_id: int, durations: float,
            image_id: Optional[str] = None, width: Optional[int] = None,
            height: Optional[int] = None, angle: Optional[int] = 0,
            num: Optional[int] = 0, image: Optional[PriPageImageData] = None,
            # readable: Optional[List[Union[Paragraph, TableText]]] = None,
            content: Optional[List[Union[PriPageContentTextLine, PriPageContentImage]]] = None,
            structured: Optional[List[Union[PriPageStructuredTable]]] = None):
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

class Metrics:
    def __init__(self, document_type: str, total_page_number: int,
            valid_page_number: int, paragraph_number: int, character_number: int):
        self.document_type = document_type
        self.total_page_number = total_page_number
        self.valid_page_number = valid_page_number
        self.paragraph_number = paragraph_number
        self.character_number = character_number

class XToMarkdownOutput:
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

def parse_x_to_markdown_output(data: dict) -> XToMarkdownOutput:
    result_data = data.get('result', {})
    metrics_data = data.get('metrics', {})

    def filter_table_data(s):
        allowed_keys = {'type', 'pos', 'rows', 'cols',
                        'columns_width', 'rows_height',
                        'cells', 'sub_type', 'is_continue'}
        return {key: s[key] for key in allowed_keys if key in s}

    pages = [
        PriPage(
            status=page.get('status', 'success'),
            page_id=page.get('page_id', 0),
            durations=page.get('durations', 0.0),
            image_id=page.get('image_id'),
            width=page.get('width'),
            height=page.get('height'),
            angle=page.get('angle', 0),
            num=page.get('num', 0),
            image=PriPageImageData(**page.get('image', {})),
            content=[PriPageContentTextLine(**c) if c.get('type') == 'line' else
                     PriPageContentImage(**c)
                     for c in page.get('content', [])],
            structured=[
                PriPageStructuredTable(**filter_table_data(s))
                for s in page.get('structured', [])
                if s.get('type') == 'table'
            ]
        )
        for page in result_data.get('pages', [])
    ]

    result = {
        'src_page_count': result_data.get('src_page_count', 0),
        'markdown': result_data.get('markdown', ''),
        'pages': pages
    }

    metrics = Metrics(
        document_type=metrics_data.get('document_type', 'unknown'),
        total_page_number=metrics_data.get('total_page_number', 0),
        valid_page_number=metrics_data.get('valid_page_number', 0),
        paragraph_number=metrics_data.get('paragraph_number', 0),
        character_number=metrics_data.get('character_number', 0)
    )

    return XToMarkdownOutput(
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

class SimpleTextInParserEngine:
    def __init__(self):
        # 提供默认值给 XToMarkdownOutput 的构造函数
        self.pri_document = XToMarkdownOutput(
            version="1.0.0",
            duration=0,
            result={},
            metrics=Metrics(
                document_type="unknown",
                total_page_number=0,
                valid_page_number=0,
                paragraph_number=0,
                character_number=0
            )
        )

    @staticmethod
    def createAndStartTextInParserEngine():
        return SimpleTextInParserEngine()

    def parse(self, json_path):
        json_file_path = Path(json_path)
        with json_file_path.open('r', encoding='utf-8') as f:
            try:
                data = json.load(f)
            except json.JSONDecodeError as e:
                print(f"JSON 解析错误: {e}")
                return -1  # 错误返回 -1

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

    def findTables(self,page_id: int) -> List[PriPageStructuredTable]:
        page = next((p for p in self.pri_document.result['pages'] if p.page_id == page_id), None)

        if not page:
            raise ValueError(f"Page with page_id {page_id} not found.")

        content_map = {c.id: c for c in page.content}

        new_tables = []

        for table in page.structured:
            if isinstance(table, dict):
                table = PriPageStructuredTable(**table)

            if isinstance(table, PriPageStructuredTable):
                new_cells = []
                for cell in table.cells:
                    if isinstance(cell, dict):
                        cell = PriPageStructuredTableTableCell(**cell)

                    new_content = []
                    for item in cell.content:
                        if isinstance(item, dict):
                            if item.get('type') == 'textblock':
                                item = PriPageStructuredTableTableCellTextBlock(**item)
                            elif item.get('type') == 'imageblock':
                                item = PriPageStructuredTableTableCellImageBlock(**item)

                        if isinstance(item, PriPageStructuredTableTableCellTextBlock):
                            resolved_content = [content_map[content_id] for content_id in item.content]
                            new_content.append(PriPageStructuredTableTableCellTextBlock(
                                type=item.type,
                                pos=item.pos,
                                content=resolved_content,
                                sub_type=item.sub_type,
                                is_continue=item.is_continue
                            ))

                        elif isinstance(item, PriPageStructuredTableTableCellImageBlock):
                            resolved_content = [content_map[content_id] for content_id in item.content]
                            new_content.append(PriPageStructuredTableTableCellImageBlock(
                                type=item.type,
                                pos=item.pos,
                                zorder=item.zorder,
                                content=resolved_content
                            ))

                    new_cell = PriPageStructuredTableTableCell(
                        row=cell.row,
                        col=cell.col,
                        pos=cell.pos,
                        content=new_content,
                        row_span=cell.row_span,
                        col_span=cell.col_span
                    )
                    new_cells.append(new_cell)

                new_table = PriPageStructuredTable(
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

    def getOrigin(self):
        return self.pri_document


