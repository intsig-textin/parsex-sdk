import unittest
import sys
import json
import subprocess
from pathlib import Path
import cv2



# 加载配置
config = json.load(open("config.json"))

if config.get("use_local_source"):
    # 使用本地的源码，方便本地修改调试ParserX
    parent_dir = Path(__file__).resolve().parent.parent
    sys.path.insert(0, str(parent_dir))
    import TextInParseX.TextInParseX as px
else:
    # 使用pip包的TextInParseX
    import TextInParseX as px

class TestPdf2MdParserEngine(unittest.TestCase):
    def setUp(self):
        pass

    def test_parseX(self):
        # 如果你已经有调用文档解析的结果，可以不需要执行get_result_via_curl， 直接指定result_json_path的路径进行解析
        app_id = config.get('app_id', '":"###########################"')
        secret_code = config.get('secret_code', "###########################")
        api_url = config.get('api_url')
        download_image_url = config.get('download_image_url')
        draw_pos = config.get('draw_pos')

        pdf_file_path = "file/example.pdf"
        
        # 初始化解析器
        parseX_client = px.ParseXClient(app_id, secret_code)
        result = parseX_client.begin_analyze_document_from_url(pdf_file_path)

        json_file = 'test_json/example.json'
        with open(json_file, 'r') as fr:
            json_result = json.load(fr)

        # 调用解析方法
        result = parseX_client.begin_analyze_document_from_json(json_result)
        
        # 检查解析结果是否为预期
        self.assertIsInstance(result, px.Document)

        # 获取文档总页数
        print(f"Total valid page size: {len(result.pages)}")
        print("\n")

        print('Markdown:')
        print(result.all_markdown)
        print("\n")

        print("All text in document:")
        parseX_client.print_all_elements(result.all_text, 0, 1000)
        print("\n")
        print(f"Total tables in document: {len(result.all_tables)}")
        
        for index, table in enumerate(result.all_tables):
            print(f"Table {index}:")
            parseX_client.print_all_elements(table)
            print("\n")
        
    
        print(f"Total paragraphs in document: {len(result.all_paragraphs)}")
        for p_idx, each_paragraph in enumerate(result.all_paragraphs):
            print(f"\n--- Paragraph {p_idx}/{len(result.all_paragraphs)} ---")
            print(f"Paragraph position: {each_paragraph.pos}")
            for l_idx, each_line in enumerate(each_paragraph.lines):
                print(f"  Line {l_idx}/{len(each_paragraph.lines)}")
                print(f"    Line positions: {each_line.pos}")
                print(f"  Line text: {each_line.text}")
    
        print(f"Total images in document: {len(result.all_images)}")
        for index, image in enumerate(result.all_images):
            print(f"Image {index}:")
            parseX_client.print_all_elements(image)
            print("\n")

        all_images_cv_mat = result.get_all_images_cv_mat()
        print(f"Total images (as cv::Mat) in document: {len(all_images_cv_mat)}")
        for idx, mat in enumerate(all_images_cv_mat):
            print(f"Image {idx} (cv::Mat) shape: {mat.shape}")

        # 遍历每一页
        #页的索引指向pdf和文档的页，按照页数的规则，从1开始； table等版面元素的索引默认程序读取的规则，从0开始
        for page in result.pages:
            print(f"=== Page {page.page_id} === Page width {page.width} Page height {page.height}")
            print("\n")
            print (f"Page Markdown\n: {page.markdown}")
            for index, table in enumerate(page.tables):
                print(f"Table {index}:")
                cells = table.cells
                parseX_client.print_all_elements(table)
                print("\n")
   
            for index, image in enumerate(page.images):
                print(f"Image {index}:")
                parseX_client.print_all_elements(image)  # 限定只能打印前50(默认)个字符        
                print("\n")
    
            images_cv_mat = page.get_images_cv_mat()
            print(f"Total images (as cv::Mat) in page {page.page_id}: {len(images_cv_mat)}")
            for idx, mat in enumerate(images_cv_mat):
                print(f"Image {idx} (cv::Mat) shape: {mat.shape}")
                print("\n")
   
            print("Text:")
            parseX_client.print_all_elements(page.paragraph_text, 0, 1000)  # 限定只能打印前1000个字符
            print("\n")
            # 获取当前页的段落
    
            print(f"Total paragraphs: {len(page.paragraphs)}")
            for p_idx, each_paragraph in enumerate(page.paragraphs):
                print(f"\n--- Paragraph {p_idx}/{len(page.paragraphs)} ---")
                print(f"Paragraph position: {each_paragraph.pos}")
                
                for l_idx, each_line in enumerate(each_paragraph.lines):
                    print(f"  Line {l_idx}/{len(each_paragraph.lines)}")
                    print(f"    Line positions: {each_line.pos}")
                    print(f"  Line text: {each_line.text}")
                print('Finished getting paragraphs')
                print("\n\n")
            

        if draw_pos == True and download_image_url is not None:
            for page in result.pages:
                page_img = parseX_client.download_image_from_url(download_image_url, page.image_id)
                if page_img is None:
                    continue
                for index, table in enumerate(page.tables):
                    cells = table.cells
                    for each_cell in cells:
                        cv2.rectangle(page_img, (each_cell.pos[0], each_cell.pos[1]), (each_cell.pos[4], each_cell.pos[5]), (0, 0, 255), 1)
            
                for index, image in enumerate(page.images):
                    cv2.rectangle(page_img, (image.pos[0], image.pos[1]), (image.pos[4], image.pos[5]), (0, 255, 255), 1)

                for p_idx, each_paragraph in enumerate(page.paragraphs):
                    cv2.rectangle(page_img, (each_paragraph.pos[0], each_paragraph.pos[1]), (each_paragraph.pos[4], each_paragraph.pos[5]), (0, 255, 0), 5)
                    for l_idx, each_line in enumerate(each_paragraph.lines):
                        cv2.rectangle(page_img, (each_line.pos[0], each_line.pos[1]), (each_line.pos[4], each_line.pos[5]), (255, 0, 0), 1)

                cv2.imwrite('image_result_{}.jpg'.format(page.page_id), page_img)
            
        # 保存表格为excel
        parseX_client.save_tables_as_excel(result.all_tables, 'tables_result.xlsx')

if __name__ == '__main__':
    unittest.main()
