import unittest
import sys
import json
from pathlib import Path

config = json.load(open("config.json"))

if config.get("use_local_source"):
    # 使用本地的源码，方便本地修改调试text_in_parser
    parent_dir = Path(__file__).resolve().parent.parent
    sys.path.insert(0, str(parent_dir))
    from pdf_parser_engine.pdf_parser_engine import SimpleTextInParserEngine, XToMarkdownOutput
else:
    # 使用pip包的text_in_parser
    from pdf_parser_engine import SimpleTextInParserEngine, XToMarkdownOutput


class TestSimpleTextInParserEngine(unittest.TestCase):
    def test_parse_json(self):
        # 初始化解析器
        parser = SimpleTextInParserEngine.createAndStartTextInParserEngine()

        # 测试文件路径
        test_json_path = 'test_json/test-2333.json'

        # 调用解析方法
        parser.parse(test_json_path)

        # 检查解析结果是否为预期
        self.assertIsInstance(parser.pri_document, XToMarkdownOutput)

        # 假设有 n 页（你可以根据实际情况设置页数）
        total_pages = parser.getPageSize()
        print(f"Total valid page size: {total_pages}")
        print("\n")

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
            # 当前页的markdown
            # findParagraph(page_id)
            #
            text = parser.findText(page_id)
            print("Text:")
            parser.print_all_elements(text, 0, 1000) # 限定只能打印前1000个字符
            print("\n")

            markdown_details = parser.getMarkdown(page_id)
            print('markdown_details is:\n', markdown_details)

            print('Start to get paragraphs')

            # 获取段落
            paragraphs = parser.getParagraph(page_id)
            print(f"Total paragraphs: {len(paragraphs)}")

            for p_idx, each_paragraph in enumerate(paragraphs):
                print(f"\n--- Paragraph {p_idx + 1}/{len(paragraphs)} ---")
                print(f"Paragraph position: {each_paragraph.pos}")

                for l_idx, each_line in enumerate(each_paragraph.lines):
                    print(f"  Line {l_idx + 1}/{len(each_paragraph.lines)}")
                    print(f"    Character positions: {each_line.char_pos}")

            print('Finished getting paragraphs')

if __name__ == '__main__':
    print('start to unittest.main')
    unittest.main()
