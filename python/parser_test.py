import textin_parser
import os

# 错误码与中文解释的映射字典
ERROR_CODES = {
    0: "操作成功",
    1: "操作失败",
    2: "无效的参数",
    3: "内部错误",
    4: "页面为空",
    5: "未有前置的解析",
    6: "缓冲区为空",
    7: "ID超出范围",
    8: "表格为空",
    9: "文件未找到"
}

def get_error_message(error_code):
    return ERROR_CODES.get(error_code, f"未知错误（代码: {error_code})")

def log_message(message, logfile, indent_level=0):
    with open(logfile, 'a', encoding='utf-8') as f:
        indent = ' ' * (indent_level * 4)
        f.write(indent + message + '\n')

def print_pos(pos, logfile, indent_level=0):
    log_message(f"位置: ({pos.x1}, {pos.y1}), ({pos.x2}, {pos.y2}), ({pos.x3}, {pos.y3}), ({pos.x4}, {pos.y4})", logfile, indent_level)

def print_text_content(t_contents, logfile, indent_level=1):
    for i, text_content in enumerate(t_contents):
        log_message(f"文本内容 {i + 1}:", logfile, indent_level)
        log_message(f"文本: {text_content.text}", logfile, indent_level + 1)
        log_message(f"方向: {text_content.direction}", logfile, indent_level + 1)
        log_message(f"分数: {text_content.score}", logfile, indent_level + 1)
        log_message(f"角度: {text_content.angle}", logfile, indent_level + 1)
        log_message(f"字符位置:", logfile, indent_level + 1)
        for pos in text_content.char_pos:
            print_pos(pos, logfile, indent_level + 2)
        log_message(f"字符候选:", logfile, indent_level + 1)
        for cand in text_content.char_cand:
            log_message(f"候选: {cand.cand_str}", logfile, indent_level + 2)
        log_message(f"字符候选分数:", logfile, indent_level + 1)
        for cand_score in text_content.char_cand_score:
            log_message(f"分数: {cand_score.score}", logfile, indent_level + 2)

def print_image_content(i_contents, logfile, indent_level=1):
    for i, image_content in enumerate(i_contents):
        log_message(f"图片内容 {i + 1}:", logfile, indent_level)
        log_message(f"类型: {image_content.stamp_type}", logfile, indent_level + 1)
        log_message(f"颜色: {image_content.stamp_color}", logfile, indent_level + 1)
        log_message(f"子类型: {image_content.sub_type}", logfile, indent_level + 1)
        log_message(f"大小: {image_content.size}", logfile, indent_level + 1)
        print_pos(image_content.pos, logfile, indent_level + 1)
        log_message(f"图片路径: {image_content.path}", logfile, indent_level + 1)
        log_message("", logfile)

def print_table(table, logfile, print_all, indent_level=1):
    log_message(f"表格:", logfile, indent_level)
    log_message(f"表格子类型: {table.sub_type}", logfile, indent_level + 1)
    print_pos(table.pos, logfile, indent_level + 1)
    log_message(f"行数: {table.rows}, 列数: {table.cols}", logfile, indent_level + 1)
    log_message(f"列宽: {table.colums_width}", logfile, indent_level + 1)
    log_message(f"行高: {table.rows_height}", logfile, indent_level + 1)
    log_message(f"是否连续: {table.is_continue}", logfile, indent_level + 1)
    if not print_all:
        return
    log_message(f"单元格:", logfile, indent_level + 1)
    for i, cell in enumerate(table.cells):
        log_message(f"单元格 {i + 1}: 行={cell.row}, 列={cell.col}", logfile, indent_level + 2)
        print_pos(cell.pos, logfile, indent_level + 2)
        log_message(f"文本块:", logfile, indent_level + 2)
        for block in cell.t_blocks:
            print_text_content(block.t_contents, logfile, indent_level + 3)
        log_message(f"图片块:", logfile, indent_level + 2)
        for block in cell.i_blocks:
            print_image_content(block.i_contents, logfile, indent_level + 3)
        log_message("--------单元格分隔符----------------------", logfile, indent_level + 2)

def print_page(page, logfile, indent_level=0):
    log_message(f"页面 ID: {page.page_id}", logfile, indent_level)
    log_message(f"页面状态: {page.page_status}", logfile, indent_level + 1)
    log_message(f"页面图像 ID: {page.page_image_id}", logfile, indent_level + 1)
    log_message(f"页面持续时间: {page.page_durations}", logfile, indent_level + 1)
    log_message(f"页面中的表格数量: {len(page.page_tables)}", logfile, indent_level + 1)
    for i, table in enumerate(page.page_tables):
        log_message(f"表格 {i + 1}:", logfile, indent_level + 1)
        print_table(table, logfile, True, indent_level + 2)
        log_message("", logfile)

def main():
    logfile = "output.log"
    if os.path.exists(logfile):
        os.remove(logfile)
    # 第一步先获取一个用来解析的对象实例
    engine = textin_parser.ITextInParserEngine.createAndStartTextInParserEngine()
    # 第二步用第一步获取到的实例解析一个存在的json文件，这个json一定是通过x_2_md API获取的，不然会失败
    result = engine.parseFile("chinese-tables.json")
    if result != 0:
        log_message(f"解析文件失败，错误码: {result} ({get_error_message(result)})", logfile)
        return

    result, page_size = engine.getPageSize()
    log_message(f"页面数量: {page_size}", logfile)

    if page_size > 0:
        # 获取具体页的表格数量
        result, table_size = engine.getPageTablesSize(0)
        log_message(f"第一个页面中的表格数量: {table_size}", logfile)
        # 你可以直接获取具体哪一页中所有的表格信息，第一个参数为页面序号，第二个参数为当前页的表格数量
        result, tables = engine.findTables(0, table_size)
        if result == 0:
            log_message(f"成功获取表格信息，表格数量: {len(tables)}", logfile)
            for i, table in enumerate(tables):
                log_message(f"表格 {i + 1}: 行数={table.rows}, 列数={table.cols}", logfile)
                print_table(table, logfile, False, indent_level=1)
        else:
            log_message(f"获取表格信息失败，错误码: {result} ({get_error_message(result)})", logfile)


    # 你也可以通过直接获取document结构访问PDF中的元素
    document = engine.getDocument()
    log_message(f"文档加密状态: {document.encrypted_state}", logfile)
    log_message(f"文档页面数量: {document.page_size}", logfile)

    # 遍历PDF中的每一页并打印每一页的元素信息
    for i, page in enumerate(document.pages):
        log_message(f"页面 {i + 1}: ID={page.page_id}, 表格数量={page.table_size}", logfile)
        print_page(page, logfile, indent_level=1)
        log_message("", logfile)

    # 使用完记得释放资源，避免不必要的资源占用
    engine.release()

if __name__ == "__main__":
    main()
