import ParseX as px
import json

json_file = 'test_json/example.json'
with open(json_file, 'r') as fr:
    json_result = json.load(fr)
    
parser_client = px.ParseXClient()
result = parser_client.begin_analyze_document_from_json(json_result)
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
    
    print(f"Total paragraphs: {len(page.paragraphs)}")
    for p_idx, each_paragraph in enumerate(page.paragraphs):
        print(f"\n--- Paragraph {p_idx + 1}/{len(page.paragraphs)} ---")
        print(f"Paragraph position: {each_paragraph.pos}")
        for l_idx, each_line in enumerate(each_paragraph.lines):
            print(f"  Line {l_idx + 1}/{len(each_paragraph.lines)}")
            print(f"    Character positions: {each_line.char_pos}")
    print('Finished getting paragraphs')
    print("\n\n")
