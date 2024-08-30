import unittest
import sys
import json
import subprocess
from pathlib import Path

# 加载配置
config = json.load(open("config.json"))

if config.get("use_local_source"):
    # 使用本地的源码，方便本地修改调试ParserX
    parent_dir = Path(__file__).resolve().parent.parent
    sys.path.insert(0, str(parent_dir))
    import TextInParseX.TextInParseX as px
else:
    # 使用pip包的ParseGenius
    import TextInParseX as px

class TestPdf2MdParserEngine(unittest.TestCase):
    def setUp(self):
        pass

    def test_parse_json(self):
        # 如果你已经有调用文档解析的结果，可以不需要执行get_result_via_curl， 直接指定result_json_path的路径进行解析
        app_id = "#########################"
        secret_code = "#############################"
        
        pdf_file_path = "file/example.pdf"
        #api_url = 'https://api.textin.com/ai/service/v1/pdf_to_markdow?markdown_details=1&apply_document_tree=1&page_details=1'

        # 初始化解析器
        parser_client = px.ParseXClient(app_id, secret_code)
        result = parser_client.begin_analyze_document_from_url(pdf_file_path)

        json_file = 'test_json/example.json'
        with open(json_file, 'r') as fr:
            json_result = json.load(fr)

        # 调用解析方法
        result = parser_client.begin_analyze_document_from_json(json_result)
        
        # 检查解析结果是否为预期
        self.assertIsInstance(result, px.Document)

        # 获取文档总页数
        print(f"Total valid page size: {len(result.pages)}")
        print("\n")

        # 遍历每一页
        for page in result.pages:
            print(f"=== Page {page.page_id + 1} ===")
            print("\n")

            # 循环打印每个表格对象
            for index, table in enumerate(page.tables):
                print(f"Table {index + 1}:")
                parser_client.print_all_elements(table)
                print("\n")
       
            # 循环打印每个图片对象
            for index, image in enumerate(page.images):
                print(f"Image {index + 1}:")
                parser_client.print_all_elements(image)  # 限定只能打印前50(默认)个字符
                print("\n")

            # 获取当前页面的图片的 cv::Mat 数组
            images_cv_mat = page.get_images_cv_mat()
            print(f"Total images (as cv::Mat) in page {page.page_id + 1}: {len(images_cv_mat)}")
            for idx, mat in enumerate(images_cv_mat):
                print(f"Image {idx + 1} (cv::Mat) shape: {mat.shape}")
            print("\n")

           
            # 打印当前页面的文本信息
            print("Text:")
            parser_client.print_all_elements(page.paragraph_text, 0, 1000)  # 限定只能打印前1000个字符
            print("\n")

            # 获取当前页的段落
            #print(f"Total paragraphs: {len(page.paragraphs)}")

            for p_idx, each_paragraph in enumerate(page.paragraphs):
                print(f"\n--- Paragraph {p_idx + 1}/{len(page.paragraphs)} ---")
                print(f"Paragraph position: {each_paragraph.pos}")

                for l_idx, each_line in enumerate(each_paragraph.lines):
                    print(f"  Line {l_idx + 1}/{len(each_paragraph.lines)}")
                    print(f"    Character positions: {each_line.char_pos}")

            print('Finished getting paragraphs')
            print("\n\n")

        # 测试获取整个文档中的所有表格
        all_tables = result.all_tables
        print(f"Total tables in document: {len(all_tables)}")
        self.assertGreater(len(all_tables), 0, "There should be at least one table in the document")
        for index, table in enumerate(all_tables):
            print(f"Table {index + 1}:")
            parser_client.print_all_elements(table)
            print("\n")

        # 测试获取整个文档中的所有段落
        all_paragraphs = result.all_paragraphs
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
        all_images = result.all_images
        print(f"Total images in document: {len(all_images)}")
        self.assertGreater(len(all_images), 0, "There should be at least one image in the document")
        for index, image in enumerate(all_images):
            print(f"Image {index + 1}:")
            parser_client.print_all_elements(image)
            print("\n")

        # 测试获取整个文档中的所有图片的 cv::Mat 数组
        all_images_cv_mat = result.get_all_images_cv_mat()
        print(f"Total images (as cv::Mat) in document: {len(all_images_cv_mat)}")
        for idx, mat in enumerate(all_images_cv_mat):
            print(f"Image {idx + 1} (cv::Mat) shape: {mat.shape}")
        print("\n")

        # 测试获取整个文档中的所有文本
        print("All text in document:")
        self.assertGreater(len(result.all_text), 0, "There should be some text in the document")
        parser_client.print_all_elements(result.all_text, 0, 1000)
        print("\n")

        # 测试获取整个文档中的所有 Markdown 内容
        markdown_details = result.all_markdown
        print('Markdown details:')
        print(markdown_details)
        print("\n")


if __name__ == '__main__':
    unittest.main()
