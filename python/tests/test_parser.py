import unittest
import sys
import json
import subprocess
from pathlib import Path

# 加载配置
config = json.load(open("config.json"))

if config.get("use_local_source"):
    # 使用本地的源码，方便本地修改调试ParseGenius
    parent_dir = Path(__file__).resolve().parent.parent
    sys.path.insert(0, str(parent_dir))
    import ParseGenius.ParseGenius
else:
    # 使用pip包的ParseGenius
    import ParseGenius

class TestPdf2MdParserEngine(unittest.TestCase):

    @staticmethod
    def get_result_via_curl(pdf_file_path, output_result_path, app_id, secret_code):
        # curl 命令的各项参数
        api_url = "https://api.textin.com/ai/service/v1/pdf_to_markdow?markdown_details=1&apply_document_tree=1&page_details=1&markdown_details=1"

        # 执行 curl 命令
        curl_command = [
            "curl",
            "--location", "--request", "POST", api_url,
            "--header", f"x-ti-app-id: {app_id}",
            "--header", f"x-ti-secret-code: {secret_code}",
            "--data-binary", f"@{pdf_file_path}",
            "--output", output_result_path
        ]

        result = subprocess.run(curl_command, capture_output=True, text=True)

        if result.returncode != 0:
            print(f"Error occurred while executing curl: {result.stderr}")
            exit(0)
            #raise Exception("Failed to download JSON data using curl")
            

    def setUp(self):
        pass
        # 在测试开始之前下载 通过pdf_to_markdown的公有云API下载JSON 数据
        # self.download_json_via_curl()

    def test_parse_json(self):
        # 初始化解析器
        parser = ParseGenius.Pdf2MdParserEngine.create_parse_genius()
        
        # 如果你已经有调用文档解析的结果，可以不需要执行get_result_via_curl， 直接指定result_json_path的路径进行解析
        app_id = "8b1666c07db002663f3b08533d140952"
        secret_code = "1a82651b1b11a9f9bcd7cc7bcf7c7820"
        pdf_file_path = "file/example.pdf"
        # 待解析的文件路径
        result_json_path = "test_json/example.json"

        TestPdf2MdParserEngine.get_result_via_curl(pdf_file_path, result_json_path, app_id, result_json_path)
         
        # 调用解析方法
        parse_result = parser.parse(result_json_path)
        self.assertEqual(parse_result, 0, "Parsing should return 0 on success")

        # 检查解析结果是否为预期
        self.assertIsInstance(parser.pri_document, ParseGenius.Document)

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
