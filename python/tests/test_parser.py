import unittest
import sys
import json
from pathlib import Path

# 加载配置
config = json.load(open("config.json"))

if config.get("use_local_source"):
    # 使用本地的源码，方便本地修改调试ParseGenius
    parent_dir = Path(__file__).resolve().parent.parent
    sys.path.insert(0, str(parent_dir))
    from ParseGenius.ParseGenius import Pdf2MdParserEngine, Document
else:
    # 使用pip包的ParseGenius
    from ParseGenius import Pdf2MdParserEngine, Document

class TestPdf2MdParserEngine(unittest.TestCase):
    def test_parse_json(self):
        # 初始化解析器
        parser = Pdf2MdParserEngine.create_parse_genius()

        # 测试文件路径
        test_json_path = 'test_json/test-2333.json'

        # 调用解析方法
        parse_result = parser.parse(test_json_path)
        self.assertEqual(parse_result, 0, "Parsing should return 0 on success")

        # 检查解析结果是否为预期
        self.assertIsInstance(parser.pri_document, Document)

        # 获取文档总页数
        total_pages = parser.get_page_size()
        print(f"Total valid page size: {total_pages}")
        print("\n")

        # 遍历每一页
        for page_id in range(total_pages):
            print(f"=== Page {page_id + 1} ===")
            print("\n")

            # 获取当前页面的表格数组
            tables = parser.find_tables(page_id)

            # 循环打印每个表格对象
            for index, table in enumerate(tables):
                print(f"Table {index + 1}:")
                parser.print_all_elements(table)
                print("\n")

            # 获取当前页面的图片数组
            images = parser.get_images(page_id)

            # 循环打印每个图片对象
            for index, image in enumerate(images):
                print(f"Image {index + 1}:")
                parser.print_all_elements(image)  # 限定只能打印前50(默认)个字符
                print("\n")

            # 获取当前页面的文本信息
            text = parser.get_text(page_id)
            print("Text:")
            parser.print_all_elements(text, 0, 1000)  # 限定只能打印前1000个字符
            print("\n")

            # 获取当前页面的markdown
            markdown_details = parser.get_markdown(page_id)
            print('Markdown details:')
            print(markdown_details)
            print("\n")

            print('Start to get paragraphs')

            # 获取段落
            paragraphs = parser.get_paragraph(page_id)
            print(f"Total paragraphs: {len(paragraphs)}")

            for p_idx, each_paragraph in enumerate(paragraphs):
                print(f"\n--- Paragraph {p_idx + 1}/{len(paragraphs)} ---")
                print(f"Paragraph position: {each_paragraph.pos}")

                for l_idx, each_line in enumerate(each_paragraph.lines):
                    print(f"  Line {l_idx + 1}/{len(each_paragraph.lines)}")
                    print(f"    Character positions: {each_line.char_pos}")

            print('Finished getting paragraphs')
            print("\n\n")

        # 测试获取整个文档中的所有表格
        all_tables = parser.find_all_tables()
        print(f"Total tables in document: {len(all_tables)}")
        self.assertGreater(len(all_tables), 0, "There should be at least one table in the document")
        for index, table in enumerate(all_tables):
            print(f"Table {index + 1}:")
            parser.print_all_elements(table)
            print("\n")

        # 测试获取整个文档中的所有段落
        all_paragraphs = parser.get_all_paragraphs()
        print(f"Total paragraphs in document: {len(all_paragraphs)}")
        self.assertGreater(len(all_paragraphs), 0, "There should be at least one paragraph in the document")
        for p_idx, each_paragraph in enumerate(all_paragraphs):
            print(f"\n--- Paragraph {p_idx + 1}/{len(all_paragraphs)} ---")
            print(f"Paragraph position: {each_paragraph.pos}")
            for l_idx, each_line in enumerate(each_paragraph.lines):
                print(f"  Line {l_idx + 1}/{len(each_paragraph.lines)}")
                print(f"    Character positions: {each_line.char_pos}")

        print('Finished getting all paragraphs')
        print("\n\n")

        # 测试获取整个文档中的所有图片
        all_images = parser.get_all_images()
        print(f"Total images in document: {len(all_images)}")
        self.assertGreater(len(all_images), 0, "There should be at least one image in the document")
        for index, image in enumerate(all_images):
            print(f"Image {index + 1}:")
            parser.print_all_elements(image)
            print("\n")

        # 测试获取整个文档中的所有文本
        all_text = parser.get_all_text()
        print("All text in document:")
        self.assertGreater(len(all_text), 0, "There should be some text in the document")
        parser.print_all_elements(all_text, 0, 1000)
        print("\n")

        # 测试获取整个文档中的所有 Markdown 内容
        all_markdown = parser.get_all_markdown()
        print("All markdown content in document:")
        self.assertGreater(len(all_markdown), 0, "There should be some markdown content in the document")
        print(all_markdown)


if __name__ == '__main__':
    unittest.main()
