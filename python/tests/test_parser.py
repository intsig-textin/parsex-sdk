import unittest
from text_in_parser_engine.text_in_parser_engine import SimpleTextInParserEngine, XToMarkdownOutput

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

        # 获取第一页的表格数组
        tables = parser.findTables(0)

        # 循环打印每个表格对象
        for index, table in enumerate(tables):
            print(f"Table {index + 1}:")
            parser.print_all_elements(table)
            print("\n")

if __name__ == '__main__':
    unittest.main()
