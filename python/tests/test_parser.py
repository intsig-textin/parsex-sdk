import unittest
import sys
from pathlib import Path

# # 获取父目录路径并添加到 sys.path
# parent_dir = Path(__file__).resolve().parent.parent
# sys.path.insert(0, str(parent_dir))

# # 现在可以导入父目录下的模块
# from text_in_parser_engine.text_in_parser_engine import SimpleTextInParserEngine, XToMarkdownOutput
from text_in_parser_engine import SimpleTextInParserEngine, XToMarkdownOutput


class TestSimpleTextInParserEngine(unittest.TestCase):
    def test_parse_json(self):
        # 初始化解析器
        parser = SimpleTextInParserEngine.createAndStartTextInParserEngine()

        # 测试文件路径
        test_json_path = 'chinese-tables.json'

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
            text = parser.findText(page_id)
            print("Text:")
            parser.print_all_elements(text, 0, 1000) # 限定只能打印前1000个字符
            print("\n")

if __name__ == '__main__':
    unittest.main()
