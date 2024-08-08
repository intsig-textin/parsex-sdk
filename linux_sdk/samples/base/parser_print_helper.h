#pragma once

#include <iostream>
#include "../../include/ITextInParserEngine.h"

using namespace intsig::textin::parser_engine;
using namespace intsig::textin::linuxsdk;

constexpr const size_t kMaxPrintCell = 5;
constexpr const size_t kMaxPrintTable = 5;

void print_pos(const Pos& pos) {
    std::cout << std::endl;
    std::cout << "Top-left (x1, y1): (" << pos.x1 << ", " << pos.y1 << ")";
    std::cout << ", Top-right (x2, y2): (" << pos.x2 << ", " << pos.y2 << ")";
    std::cout << ", Bottom-right (x3, y3): (" << pos.x3 << ", " << pos.y3 << ")";
    std::cout << ", Bottom-left (x4, y4): (" << pos.x4 << ", " << pos.y4 << ")";
}

void print_document_metadata(const MetaData& metadata) {
    std::cout << "Document Version: " << metadata.version << std::endl;
    std::cout << "Document Duration: " << metadata.duration << std::endl;
    std::cout << "Document MarkDown: " << metadata.markdown << std::endl;
}

void print_document_metrics(const Metrics& metrics) {
    std::cout << "Document Type: " << metrics.document_type << std::endl;
    std::cout << "Document Page Total: " << metrics.page_total << std::endl;
}

void print_page_info(const Page& page, int pageIndex) {
    std::cout << "Page [" << pageIndex << "]" << std::endl;
    std::cout << "Page Status: " << page.page_status << std::endl;
    std::cout << "Page Image Id: " << page.page_image_id << std::endl;
    std::cout << "Page Id: " << page.page_id << std::endl;
    std::cout << "Page Durations: " << page.page_durations << std::endl;
}

void print_table_info(const Table& table, int tableIndex) {
    std::cout << "Table [" << tableIndex << "]" << std::endl;
    std::cout << "Table Sub Type: " << table.sub_type << std::endl;
    std::cout << "Table Rows: " << table.rows << std::endl;
    std::cout << "Table Cols: " << table.cols << std::endl;

    std::cout << "Table Colums_width: ";
    for (int j = 0; j < table.colums_width_size; ++j) {
        std::cout << "width[" << j << "]: " << table.colums_width[j] << " ";
    }
    std::cout << std::endl;

    std::cout << "Table Rows_height: ";
    for (int j = 0; j < table.rows_height_size; ++j) {
        std::cout << "height[" << j << "]: " << table.rows_height[j] << " ";
    }
    std::cout << std::endl;

    std::cout << "Table Continue: " << (table.is_continue ? "True" : "False") << std::endl;
}

void print_text_block(const Textblock& textBlock, int blockIndex) {
    std::cout << "TextBlock[" << blockIndex << "]" << std::endl;
    std::cout << "TextBlock[" << blockIndex << "]-Continue: "
              << (textBlock.is_continue ? "True" : "False") << std::endl;
    std::cout << "TextBlock[" << blockIndex << "]-Subtype: "
              << textBlock.sub_type << std::endl;

    std::cout << "TextBlock[" << blockIndex << "]-AllText: ";
    for (int g = 0; g < textBlock.content_size; g++) {
        std::cout << textBlock.t_contents[g].text;
    }
    std::cout << std::endl;

    std::cout << "TextBlock[" << blockIndex << "]-All Direction: ";
    for (int g = 0; g < textBlock.content_size; g++) {
        std::cout << textBlock.t_contents[g].direction << " ";
    }
    std::cout << std::endl;

    std::cout << "TextBlock[" << blockIndex << "]-All Angle: ";
    for (int g = 0; g < textBlock.content_size; g++) {
        std::cout << textBlock.t_contents[g].angle << " ";
    }
    std::cout << std::endl;

    std::cout << "TextBlock[" << blockIndex << "]-All Score: ";
    for (int g = 0; g < textBlock.content_size; g++) {
        std::cout << textBlock.t_contents[g].score << " ";
    }
    std::cout << std::endl;

    std::cout << "TextBlock[" << blockIndex << "]-All Char Pos: " << std::endl;
    for (int g = 0; g < textBlock.content_size; g++) {
        for (int p = 0; p < textBlock.t_contents[g].char_pos_size; ++p) {
            std::cout << "char[" << p << "] pos: ";
            print_pos(textBlock.t_contents[g].char_pos[p]);
            std::cout << std::endl;
        }
    }
}

void print_image_block(const Imageblock& imageBlock, int blockIndex) {
    std::cout << "ImageBlock[" << blockIndex << "]" << std::endl;
    std::cout << "ImageBlock[" << blockIndex << "]-Subtype: "
              << imageBlock.sub_type << std::endl;
    std::cout << "ImageBlock[" << blockIndex << "]-Zorder: "
              << imageBlock.zorder << std::endl;
    std::cout << "ImageBlock[" << blockIndex << "]-Pos: ";
    print_pos(imageBlock.pos);
    std::cout << std::endl;
}

void print_table_cells(const Table& table) {
    for (size_t c = 0; c < std::min(static_cast<size_t>(table.cell_size), static_cast<size_t>(kMaxPrintCell)); ++c) {
        std::cout << "Cell[" << c << "]" << std::endl;
        std::cout << "Cell[" << c << "]-Pos: ";
        print_pos(table.cells[c].pos);
        std::cout << std::endl;
        std::cout << "Cell[" << c << "]-Row:" << table.cells[c].row << std::endl;
        std::cout << "Cell[" << c << "]-Col:" << table.cells[c].col << std::endl;

        // Print text blocks
        for (int t = 0; t < table.cells[c].t_block_size; ++t) {
            print_text_block(table.cells[c].t_blocks[t], t);
        }

        // Print image blocks
        for (int t = 0; t < table.cells[c].i_block_size; ++t) {
            print_image_block(table.cells[c].i_blocks[t], t);
        }
    }
}
