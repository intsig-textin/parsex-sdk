// Copyright (c) 2024 Intsig.net. All rights reserved
// Author shuwei_jing@intsig.net

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdio.h>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <algorithm> // For std::copy
#include <iterator>  // For std::begin and std::end
#include <functional> // For std::reference_wrapper
#include <nlohmann/json.hpp>

#include "TextInPaserEngine.h"

#ifdef DEBUG
#define LOG(x) std::cout << x << std::endl
#else
#define LOG(x) // No-op
#endif

namespace intsig {
namespace textin {
namespace parser_engine{

constexpr const int kCoordinateCount = 8;

// Pos create_pos() {
//     Pos pos = {0, 0, 0, 0, 0, 0, 0, 0};
//     return pos;
// }

// ImageContent create_image_content() {
//     ImageContent content = {NULL, NULL, NULL, NULL, 0, create_pos(), NULL, NULL};
//     return content;
// }

// Imageblock create_image_block() {
//     Imageblock block = {create_pos(), 0, NULL, NULL, 0};
//     return block;
// }

// TextContent create_text_content() {
//     TextContent content = {NULL, 0, 0, 0, NULL, 0, NULL, 0, NULL, 0};
//     return content;
// }

// Textblock create_text_block() {
//     Textblock block = {create_pos(), NULL, 0, NULL, 0};
//     return block;
// }

// Cell create_cell() {
//     Cell cell = {create_pos(), 0, 0, NULL, 0, NULL, 0};
//     return cell;
// }

// Table create_table() {
//     Table table = {NULL, create_pos(), 0, 0, NULL, NULL, 0, NULL, 0};
//     return table;
// }

// Page create_page() {
//     Page page = {-1, NULL, NULL, 0.0f, NULL, 0};
//     return page;
// }

// MetaData create_metadata() {
//     MetaData metadata = {NULL, NULL, 0.0f};
//     return metadata;
// }

// Metrics create_metrics() {
//     Metrics metrics = {0, 0, 0, 0, NULL};
//     return metrics;
// }

// Document create_document() {
//     Document document = {NULL, 0, create_metrics(), create_metadata(), 0};
//     return document;
// }

// Implementation of from_json involves parsing the provided response raw string
// and converting the raw string into an internal data structure according
// to the API format, ensuring that all data remains unchanged.
void print_json(const json& j) {
    std::string json_str = j.dump();
    if (json_str.length() > 100) {
        LOG(json_str.substr(0, 100) << "...");
    }
    else {
        LOG(json_str);
    }
}

void from_json(const json& j, OriPageStructuredTableCellContentTextBlock& text_block) {
    LOG("---------from json OriPageStructuredTableCellContentTextBlock---------");
    print_json(j);
    if (j.contains("pos")) j.at("pos").get_to(text_block.pos);
    if (j.contains("sub_type")) j.at("sub_type").get_to(text_block.sub_type);
    if (j.contains("continue")) j.at("continue").get_to(text_block.is_continue);
    if (j.contains("content")) j.at("content").get_to(text_block.content_ids);
}

void from_json(const json& j, OriPageStructuredTableCellContentImageBlock& image_block) {
    LOG("---------from json OriPageStructuredTableCellContentImageBlock---------");
    print_json(j);
    if (j.contains("pos")) j.at("pos").get_to(image_block.pos);
    if (j.contains("lines")) j.at("lines").get_to(image_block.line_ids);
    if (j.contains("zorder")) j.at("zorder").get_to(image_block.zorder);
    if (j.contains("content")) j.at("content").get_to(image_block.content_ids);
}

void from_json(const json& j, OriPageStructuredTableCellContent& d) {
    LOG("---------from json OriPageStructuredTableCellContent---------");
    print_json(j);

    if (!j.is_array()) {
        throw std::runtime_error("JSON is not an array in OriPageStructuredTableCellContent");
    }

    for (const auto& item : j) {
        if (!item.contains("type")) {
            throw std::runtime_error("Type not found in JSON OriPageStructuredTableCellContent");
        }

        std::string type = item.at("type").get<std::string>();
        if (type == "textblock") {
            OriPageStructuredTableCellContentTextBlock text_block;
            from_json(item, text_block);
            d.text_blocks.push_back(std::move(text_block));
        } else if (type == "image") {
            OriPageStructuredTableCellContentImageBlock image_block;
            from_json(item, image_block);
            d.image_blocks.push_back(std::move(image_block));
        } else {
            LOG("Unknown type: " + type);
        }
    }
}

void from_json(const json& j, OriPageStructuredTableCell& d) {
    LOG("---------from json OriPageStructuredTableCell---------");
    print_json(j);
    if (j.contains("row")) j.at("row").get_to(d.row);
    if (j.contains("col")) j.at("col").get_to(d.col);
    if (j.contains("row_span")) j.at("row_span").get_to(d.row_span);
    if (j.contains("col_span")) j.at("col_span").get_to(d.col_span);
    if (j.contains("pos")) j.at("pos").get_to(d.pos);
    if (j.contains("content")) j.at("content").get_to(d.content);
}

void from_json(const json& j, OriPageStructuredTable& table) {
    LOG("---------from json OriPageStructuredTable---------");
    print_json(j);
    if (j.contains("sub_type")) table.sub_type = j.at("sub_type").get<std::string>();
    if (j.contains("pos")) j.at("pos").get_to(table.pos);
    if (j.contains("rows")) table.rows = j.at("rows").get<int>();
    if (j.contains("cols")) table.cols = j.at("cols").get<int>();
    if (j.contains("columns_width")) j.at("columns_width").get_to(table.columns_width);
    if (j.contains("rows_height")) j.at("rows_height").get_to(table.rows_height);
    if (j.contains("continue")) table.continue_table = j.at("continue").get<bool>();
    if (j.contains("cells")) j.at("cells").get_to(table.cells);
}

void from_json(const json& j, OriPageStructured& d) {
    LOG("---------from json OriPageStructured---------");
    print_json(j);

    if (!j.is_array()) {
        throw std::runtime_error("JSON is not an array in OriPageStructured");
    }

    for (const auto& item : j) {
        if (!item.contains("type")) {
            throw std::runtime_error("Type not found in JSON OriPageStructured");
        }

        std::string type = item.at("type").get<std::string>();
        if (type == "table") {
            OriPageStructuredTable table;
            from_json(item, table);
            d.tables.push_back(std::move(table));
        } else {
            LOG("Unknown type: " + type);
        }
    }
}

void from_json(const json& j, OriPageContentImageData& d) {
    LOG("---------from json OriPageContentImageData---------");
    print_json(j);
    if (j.contains("base64")) j.at("base64").get_to(d.image_data.base64);
    if (j.contains("region")) j.at("region").get_to(d.image_data.region);
    if (j.contains("path")) j.at("path").get_to(d.image_data.path);
}

void from_json(const json& j, OriPageContentTextline& text_line) {
    LOG("---------from json OriPageContentTextline---------");
    print_json(j);
    if (j.contains("id")) text_line.id = j.at("id").get<int>();
    if (j.contains("text")) text_line.text = j.at("text").get<std::string>();
    if (j.contains("angle")) text_line.angle = j.at("angle").get<int>();
    if (j.contains("pos")) j.at("pos").get_to(text_line.pos);
    if (j.contains("sub_type")) text_line.sub_type = j.at("sub_type").get<std::string>();
    if (j.contains("direction")) text_line.direction = j.at("direction").get<int>();
    if (j.contains("score")) text_line.score = j.at("score").get<int>();
    if (j.contains("char_pos")) j.at("char_pos").get_to(text_line.char_pos);
    if (j.contains("char_cand")) j.at("char_cand").get_to(text_line.char_cand);
    if (j.contains("char_cand_score")) j.at("char_cand_score").get_to(text_line.char_cand_score);
}

void from_json(const json& j, OriPageContentImage& image) {
    LOG("---------from json OriPageContentImage---------");
    print_json(j);
    if (j.contains("id")) image.id = j.at("id").get<int>();
    if (j.contains("pos")) j.at("pos").get_to(image.pos);
    if (j.contains("sub_type")) image.sub_type = j.at("sub_type").get<std::string>();
    if (j.contains("stamp_type")) image.stamp_type = j.at("stamp_type").get<std::string>();
    if (j.contains("stamp_shape")) image.stamp_shape = j.at("stamp_shape").get<std::string>();
    if (j.contains("stamp_color")) image.stamp_color = j.at("stamp_color").get<std::string>();
    if (j.contains("size")) j.at("size").get_to(image.size);
    if (j.contains("data")) j.at("data").get_to(image.data);
}

void from_json(const json& j, OriPageContent& d) {
    LOG("---------from json OriPageContent---------");
    print_json(j);

    if (!j.is_array()) {
        throw std::runtime_error("JSON is not an array in OriPageContent");
    }

    for (const auto& item : j) {
        if (!item.contains("type")) {
            throw std::runtime_error("Type not found in JSON OriPageContent");
        }

        std::string type = item.at("type").get<std::string>();
        if (type == "line") {
            OriPageContentTextline text_line;
            from_json(item, text_line);
            d.text_lines.push_back(std::move(text_line));
        } else if (type == "image") {
            OriPageContentImage image;
            from_json(item, image);
            d.images.push_back(std::move(image));
        } else {
            LOG("Unknown type: " + type);
        }
    }
}

void from_json(const json& j, OriPageImage& d) {
    LOG("---------from json OriPageImage---------");
    print_json(j);
    if (j.contains("base64")) j.at("base64").get_to(d.base64);
    if (j.contains("region")) j.at("region").get_to(d.region);
    if (j.contains("path")) j.at("path").get_to(d.path);
}

void from_json(const json& j, OriPage& d) {
    LOG("---------from json OriPage---------");
    print_json(j);
    if (j.contains("status")) j.at("status").get_to(d.status);
    if (j.contains("page_id")) j.at("page_id").get_to(d.page_id);
    if (j.contains("durations")) j.at("durations").get_to(d.durations);
    if (j.contains("image_id")) j.at("image_id").get_to(d.image_id);
    if (j.contains("width")) j.at("width").get_to(d.width);
    if (j.contains("height")) j.at("height").get_to(d.height);
    if (j.contains("angle")) j.at("angle").get_to(d.angle);
    if (j.contains("num")) j.at("num").get_to(d.num);
    if (j.contains("image")) j.at("image").get_to(d.image);
    if (j.contains("content")) j.at("content").get_to(d.content);
    if (j.contains("structured")) j.at("structured").get_to(d.structured);
}

void from_json(const json& j, OriResult& d) {
    LOG("---------from json OriResult---------");
    print_json(j);
    if (j.contains("src_page_count")) j.at("src_page_count").get_to(d.src_page_count);
    if (j.contains("markdown")) j.at("markdown").get_to(d.markdown);
    if (j.contains("pages")) j.at("pages").get_to(d.pages);
}

void from_json(const json& j, OriMetrics& d) {
    LOG("---------from json OriMetrics---------");
    print_json(j);
    if (j.contains("document_type")) j.at("document_type").get_to(d.document_type);
    if (j.contains("total_page_number")) j.at("total_page_number").get_to(d.total_page_number);
    if (j.contains("valid_page_number")) j.at("valid_page_number").get_to(d.valid_page_number);
    if (j.contains("paragraph_number")) j.at("paragraph_number").get_to(d.paragraph_number);
    if (j.contains("character_number")) j.at("character_number").get_to(d.character_number);
}

void from_json(const json& j, APIResponse& d) {
    LOG("---------from json APIResponse---------");
    print_json(j);
    if (j.contains("code")) j.at("code").get_to(d.code);
    if (j.contains("duration")) j.at("duration").get_to(d.duration);
    if (j.contains("version")) j.at("version").get_to(d.version);
    if (j.contains("message")) j.at("message").get_to(d.message);
    if (j.contains("result")) j.at("result").get_to(d.result);
    if (j.contains("metrics")) j.at("metrics").get_to(d.metrics);
}

///////////////////////////////////////for external pub struct
#define MAX_PRINT_LENGTH 50

void truncate_and_print(const char* str) {
    if (strlen(str) > MAX_PRINT_LENGTH) {
        printf("%.50s...\n", str);
    } else {
        printf("%s\n", str);
    }
}

void print_pos(const Pos* pos, int indent) {
    for (int i = 0; i < indent; i++) printf(" ");
    printf("Pos: (%d, %d, %d, %d, %d, %d, %d, %d)\n",
        pos->x1, pos->y1, pos->x2, pos->y2, pos->x3, pos->y3, pos->x4, pos->y4);
}

void print_array(const char* name, const int* arr, int size, int indent) {
    for (int i = 0; i < indent; i++) printf(" ");
    printf("%s: [", name);
    for (int i = 0; i < size; i++) {
        if (i > 0) printf(", ");
        printf("%d", arr[i]);
    }
    printf("]\n");
}

void print_image_content(const ImageContent* content, int indent) {
    for (int i = 0; i < indent; i++) printf(" ");
    printf("stamp_type: ");
    truncate_and_print(content->stamp_type);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("stamp_color: ");
    truncate_and_print(content->stamp_color);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("sub_type: ");
    truncate_and_print(content->sub_type);

    print_array("size", content->size, content->size_size, indent + 2);

    print_pos(&content->pos, indent + 2);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("base64: ");
    truncate_and_print(content->base64);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("path: ");
    truncate_and_print(content->path);
}

void print_imageblock(const Imageblock* block, int indent) {
    print_pos(&block->pos, indent);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("zorder: %d\n", block->zorder);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("sub_type: ");
    truncate_and_print(block->sub_type);

    for (int i = 0; i < block->content_size; i++) {
        for (int j = 0; j < indent; j++) printf(" ");
        printf("ImageContent[%d]:\n", i);
        print_image_content(&block->i_contents[i], indent + 2);
    }
}

void print_text_content(const TextContent* content, int indent) {
    for (int i = 0; i < indent; i++) printf(" ");
    printf("text: ");
    truncate_and_print(content->text);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("direction: %d\n", content->direction);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("score: %d\n", content->score);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("angle: %d\n", content->angle);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("char_pos: [");
    for (int i = 0; i < content->char_pos_size; i++) {
        if (i > 0) printf(", ");
        print_pos(&content->char_pos[i], 0);
    }
    printf("]\n");

    for (int i = 0; i < content->char_cand_size; i++) {
        for (int j = 0; j < indent; j++) printf(" ");
        printf("Cand[%d]: ", i);
        truncate_and_print(content->char_cand[i].cand_str);
    }

    for (int i = 0; i < content->char_cand_score_size; i++) {
        for (int j = 0; j < indent; j++) printf(" ");
        printf("CandScore[%d]: %f\n", i, content->char_cand_score[i].score);
    }
}

void print_textblock(const Textblock* block, int indent) {
    print_pos(&block->pos, indent);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("sub_type: ");
    truncate_and_print(block->sub_type);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("is_continue: %d\n", block->is_continue);

    for (int i = 0; i < block->content_size; i++) {
        for (int j = 0; j < indent; j++) printf(" ");
        printf("TextContent[%d]:\n", i);
        print_text_content(&block->t_contents[i], indent + 2);
    }
}

void print_cell(const Cell* cell, int indent) {
    print_pos(&cell->pos, indent);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("row: %d\n", cell->row);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("col: %d\n", cell->col);

    for (int i = 0; i < cell->t_block_size; i++) {
        for (int j = 0; j < indent; j++) printf(" ");
        printf("Textblock[%d]:\n", i);
        print_textblock(&cell->t_blocks[i], indent + 2);
    }

    for (int i = 0; i < cell->i_block_size; i++) {
        for (int j = 0; j < indent; j++) printf(" ");
        printf("Imageblock[%d]:\n", i);
        print_imageblock(&cell->i_blocks[i], indent + 2);
    }
}

void print_table(const Table* table, int indent) {
    for (int i = 0; i < indent; i++) printf(" ");
    printf("sub_type: ");
    truncate_and_print(table->sub_type);

    print_pos(&table->pos, indent + 2);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("rows: %d\n", table->rows);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("cols: %d\n", table->cols);

    print_array("colums_width", table->colums_width, table->colums_width_size, indent + 2);
    print_array("rows_height", table->rows_height, table->rows_height_size, indent + 2);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("is_continue: %d\n", table->is_continue);

    for (int i = 0; i < table->cell_size; i++) {
        for (int j = 0; j < indent; j++) printf(" ");
        printf("Cell[%d]:\n", i);
        print_cell(&table->cells[i], indent + 2);
    }
}

void print_page(const Page* page, int indent) {
    for (int i = 0; i < indent; i++) printf(" ");
    printf("page_id: %d\n", page->page_id);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("page_status: ");
    truncate_and_print(page->page_status);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("page_image_id: ");
    truncate_and_print(page->page_image_id);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("page_durations: %f\n", page->page_durations);

    for (int i = 0; i < page->table_size; i++) {
        for (int j = 0; j < indent; j++) printf(" ");
        printf("Table[%d]:\n", i);
        print_table(&page->page_tables[i], indent + 2);
    }
}

void print_metadata(const MetaData* metadata, int indent) {
    for (int i = 0; i < indent; i++) printf(" ");
    printf("version: ");
    truncate_and_print(metadata->version);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("markdown: ");
    truncate_and_print(metadata->markdown);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("duration: %f\n", metadata->duration);
}

void print_metrics(const Metrics* metrics, int indent) {
    for (int i = 0; i < indent; i++) printf(" ");
    printf("page_total: %d\n", metrics->page_total);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("page_valid: %d\n", metrics->page_valid);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("paragraph_num: %d\n", metrics->paragraph_num);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("char_num: %d\n", metrics->char_num);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("document_type: ");
    truncate_and_print(metrics->document_type);
}

void print_document(const Document* doc, int indent) {
    for (int i = 0; i < doc->page_size; i++) {
        for (int j = 0; j < indent; j++) printf(" ");
        printf("Page[%d]:\n", i);
        print_page(&doc->pages[i], indent + 2);
    }

    print_metrics(&doc->metrics, indent + 2);
    print_metadata(&doc->metadata, indent + 2);

    for (int i = 0; i < indent; i++) printf(" ");
    printf("encrypted_state: %d\n", doc->encrypted_state);
}

#define ASSIGN_POS(pub, ori, TRANSFER_MAPPING)       \
    pub.pos = intsig::textin::parser_engine::Pos(    \
        ori.pos[TRANSFER_MAPPING::X1],               \
        ori.pos[TRANSFER_MAPPING::Y1],               \
        ori.pos[TRANSFER_MAPPING::X2],               \
        ori.pos[TRANSFER_MAPPING::Y2],               \
        ori.pos[TRANSFER_MAPPING::X3],               \
        ori.pos[TRANSFER_MAPPING::Y3],               \
        ori.pos[TRANSFER_MAPPING::X4],               \
        ori.pos[TRANSFER_MAPPING::Y4])

using intsig::textin::linuxsdk::ERROR_CODE_TYPE;

bool TextInParserEngine::LinkCellPartImage(
        const OriPageStructuredTableCellContentImageBlock& image_block,
        const std::unordered_map<int, const OriPageContentImage*> &image_map,
        int cell_i_block_index,
        intsig::textin::parser_engine::Cell *cell) {
    LOG("Function name: " << __func__);
    intsig::textin::parser_engine::Imageblock pub_i_block;
    ASSIGN_POS(pub_i_block, image_block, TRANSFER_MAPPING);
    pub_i_block.zorder = image_block.zorder;

    pub_i_block.content_size = image_block.content_ids.size();
    pub_i_block.i_contents
        = (intsig::textin::parser_engine::ImageContent*)malloc(
        pub_i_block.content_size * sizeof(
            intsig::textin::parser_engine::ImageContent));

    int block_i_content_index = 0;
    std::for_each(image_block.content_ids.begin(),
                  image_block.content_ids.end(), [&](const int id) {
        auto it = image_map.find(id);
        if (it != image_map.end()) {
            const OriPageContentImage* page_content = it->second;
            intsig::textin::parser_engine::ImageContent pub_i_content;

            pub_i_content.base64 = page_content->data.image_data.base64.c_str();
            pub_i_content.path = page_content->data.image_data.path.c_str();
            ASSIGN_POS(pub_i_content, (*page_content), TRANSFER_MAPPING);

            if (!page_content->size.empty()) {
                pub_i_content.size_size = page_content->size.size();
                pub_i_content.size
                    = (int*)malloc(pub_i_content.size_size * sizeof(int));
                std::copy(page_content->size.begin(), page_content->size.end(),
                          pub_i_content.size);
            }

            pub_i_content.stamp_type = page_content->stamp_type.c_str();
            pub_i_content.stamp_color = page_content->stamp_color.c_str();
            pub_i_content.sub_type = page_content->sub_type.c_str();

            pub_i_block.i_contents[block_i_content_index] = pub_i_content;
            block_i_content_index++;
            return true;
        } else {
            return false;
        }
    });
    cell->i_blocks[cell_i_block_index] = pub_i_block;
    return true;
}

void convertVectorToArray(const std::vector<std::vector<int>>& coordinates_vec,
        intsig::textin::parser_engine::Pos arr[], int maxCount) {
    int count = 0;
    for (const auto& coordinates : coordinates_vec) {
        if (coordinates.size() == kCoordinateCount && count < maxCount) {
            arr[count] = intsig::textin::parser_engine::Pos(
            coordinates[0], coordinates[1], coordinates[2], coordinates[3],
            coordinates[4], coordinates[5], coordinates[6], coordinates[7]);
            ++count;
        } else {
            std::cerr << "Invalid coordinate set or array limit reached" << std::endl;
        }
    }
}

template<typename T, typename U, typename V>
void convertToTypeArray(
        const std::vector<std::vector<T>>& input, U output[], V converter) {
    auto it = output;
    for (const auto& vec : input) {
        std::transform(vec.begin(), vec.end(), it, converter);
        it += vec.size();
    }
}

void convertToCharArray(
        const std::vector<std::vector<std::string>>& char_cand,
        intsig::textin::parser_engine::TextContent::Cand output[]) {
    convertToTypeArray(char_cand, output, [](const std::string& str) {
        return intsig::textin::parser_engine::TextContent::Cand{str.c_str()};
    });
}

void convertToFloatArray(
        const std::vector<std::vector<float>>& char_cand_score,
        intsig::textin::parser_engine::TextContent::CandScore output[]) {
    convertToTypeArray(char_cand_score, output, [](float score) {
        return intsig::textin::parser_engine::TextContent::CandScore{score};
    });
}

bool TextInParserEngine::LinkCellPartText(
    const OriPageStructuredTableCellContentTextBlock& text_blocks,
    const std::unordered_map<int, const OriPageContentTextline*> &text_line_map,
    int cell_t_block_index,
    intsig::textin::parser_engine::Cell *cell) {
    LOG("Function name: " << __func__);

    intsig::textin::parser_engine::Textblock pub_t_block;
    ASSIGN_POS(pub_t_block, text_blocks, TRANSFER_MAPPING);
    pub_t_block.sub_type = text_blocks.sub_type.c_str();
    pub_t_block.is_continue = text_blocks.is_continue;

    pub_t_block.content_size = text_blocks.content_ids.size();
    pub_t_block.t_contents
        = (intsig::textin::parser_engine::TextContent*)malloc(
        pub_t_block.content_size * sizeof(
            intsig::textin::parser_engine::TextContent));

    int block_t_content_index = 0;
    std::for_each(text_blocks.content_ids.begin(),
                  text_blocks.content_ids.end(), [&](const auto& id) {
        auto it = text_line_map.find(id);
        if (it != text_line_map.end()) {
            const OriPageContentTextline* page_content = it->second;
            intsig::textin::parser_engine::TextContent pub_t_content;
            pub_t_content.angle = page_content->angle;
            pub_t_content.direction = page_content->direction;
            pub_t_content.score = page_content->score;
            pub_t_content.text = page_content->text.c_str();
            if (!page_content->char_pos.empty()) {
                pub_t_content.char_pos_size = page_content->char_pos.size();
                pub_t_content.char_pos = (intsig::textin::parser_engine::Pos*)malloc(
                    pub_t_content.char_pos_size * sizeof(intsig::textin::parser_engine::Pos));
                convertVectorToArray(
                    page_content->char_pos,
                    pub_t_content.char_pos,
                    intsig::textin::parser_engine::TextContent::kMaxTextBlockCharCount);
            }

            if (!page_content->char_cand.empty()) {
                pub_t_content.char_cand_size = page_content->char_cand.size();
                pub_t_content.char_cand = (
                        intsig::textin::parser_engine::TextContent::Cand*)malloc(
                    pub_t_content.char_cand_size * sizeof(
                        intsig::textin::parser_engine::TextContent::Cand));
                convertToCharArray(
                    page_content->char_cand,
                    pub_t_content.char_cand);
            }

            if (!page_content->char_cand_score.empty()) {
                pub_t_content.char_cand_score_size = page_content->char_cand_score.size();
                pub_t_content.char_cand_score = (
                        intsig::textin::parser_engine::TextContent::CandScore*)malloc(
                    pub_t_content.char_cand_score_size * sizeof(
                        intsig::textin::parser_engine::TextContent::CandScore));
                convertToFloatArray(
                    page_content->char_cand_score,
                    pub_t_content.char_cand_score);
            }

            pub_t_block.t_contents[block_t_content_index] = pub_t_content;
            block_t_content_index++;
            return true;
        } else {
            return false;
        }
    });
    cell->t_blocks[cell_t_block_index] = pub_t_block;
    return true;
}

int TextInParserEngine::LinkTableCellAndPageContent(
    const std::vector<OriPageStructuredTableCellContentImageBlock> &image_blocks,
    const std::vector<OriPageStructuredTableCellContentTextBlock> &text_blocks,
    const OriPageContent &content,
    intsig::textin::parser_engine::Cell * pub_cell) {
    LOG("Function name: " << __func__);
    // Create a map from page_content.id to OriPageContent reference for quick lookup
    const std::vector<OriPageContentTextline> &text_lines = content.text_lines;
    const std::vector<OriPageContentImage> &images = content.images;

    if (text_lines.empty() && images.empty()) {
        LOG("Function name: " << __func__ << ", all content empty");
    }

    std::unordered_map<int, const OriPageContentTextline*> text_line_map;
    for (const auto& text_line : text_lines) {
        text_line_map[text_line.id] = &text_line;
    }

    std::unordered_map<int, const OriPageContentImage*> image_map;
    for (const auto& image : images) {
        image_map[image.id] = &image;
    }

    pub_cell->i_block_size = image_blocks.size();
    pub_cell->t_block_size = text_blocks.size();

    pub_cell->i_blocks = (intsig::textin::parser_engine::Imageblock*)malloc(
        pub_cell->i_block_size * sizeof(intsig::textin::parser_engine::Imageblock));

    pub_cell->t_blocks = (intsig::textin::parser_engine::Textblock*)malloc(
        pub_cell->t_block_size * sizeof(intsig::textin::parser_engine::Textblock));

    int cell_i_block_index = 0;
    int cell_t_block_index = 0;
    for (const auto& image_block : image_blocks) {
        if (LinkCellPartImage(image_block, image_map, cell_i_block_index, pub_cell)) {
            cell_i_block_index++;
        } else {
            LOG("Function name: " << __func__ << ", linkCellPartImage fail, cell_i_block_index: "
            << cell_i_block_index);

            return ERROR_CODE_TYPE::ERR_FAILED;
        }
    }
    for (const auto& text_block : text_blocks) {
        if (LinkCellPartText(text_block, text_line_map, cell_t_block_index, pub_cell)) {
            cell_t_block_index++;
        } else {
            LOG("Function name: " << __func__ << ", linkCellPartText fail, cell_t_block_index: "
            << cell_t_block_index);
        }
    }
    return ERROR_CODE_TYPE::ERR_OK;
}

void TextInParserEngine::ConvertDocMetaData() {
    LOG("Function name: " << __func__);
    tmp_pub_doc->metadata.version = res->version.c_str();
    tmp_pub_doc->metadata.markdown = res->result.markdown.c_str();
    tmp_pub_doc->metadata.duration = res->duration;
}

void TextInParserEngine::ConvertDocMetrics() {
    LOG("Function name: " << __func__);
    tmp_pub_doc->metrics.page_total = res->metrics.total_page_number;
    tmp_pub_doc->metrics.page_valid = res->metrics.valid_page_number;
    tmp_pub_doc->metrics.paragraph_num = res->metrics.paragraph_number;
    tmp_pub_doc->metrics.char_num = res->metrics.character_number;

    tmp_pub_doc->metrics.document_type = res->metrics.document_type.c_str();
}

 int TextInParserEngine::ConvertDocPageContent(const OriPage& page,
        intsig::textin::parser_engine::Page * pub_page) {
    LOG("Function name: " << __func__);
    return ConvertDocPageTables(page, pub_page);
    // (NOTE)JingShuwei:
    // There are many structures built into the page. Let's implement the table first
    // e.g:
    // ConvertDocPageStamps(page, pub_page);
    // ConvertDocPageParagraphs(page, pub_page);
}

// (FIXME)JingShuwei:ugly olny convter cell struct and content
int TextInParserEngine::ConverteCells(
        const OriPageStructuredTable& table_struct,
        const OriPageContent &page_content,
        intsig::textin::parser_engine::Table * pub_table) {
    LOG("Function name: " << __func__);
    if (table_struct.cells.empty() ||
        table_struct.cells.size() > intsig::textin::parser_engine::Table::kMaxCellsCount) {
         LOG("Function name: " << __func__
            << ", table struct cell size" << table_struct.cells.size());
        return ERROR_CODE_TYPE::ERR_INTERNAL_FAILED;
    }

    const std::vector<OriPageStructuredTableCell>& cells = table_struct.cells;

    pub_table->cell_size = cells.size();
    pub_table->cells = (intsig::textin::parser_engine::Cell*)malloc(
        pub_table->cell_size * sizeof(intsig::textin::parser_engine::Cell));

    int result_link = ERROR_CODE_TYPE::ERR_OK;
    for (size_t index = 0; index < cells.size(); index++) {
        intsig::textin::parser_engine::Cell pub_cell;
        const OriPageStructuredTableCell& cell = cells[index];
        ASSIGN_POS(pub_cell, cell, TRANSFER_MAPPING);
        pub_cell.row = cell.row;
        pub_cell.col = cell.col;
        const OriPageStructuredTableCellContent& cell_content = cell.content;

        const std::vector<OriPageStructuredTableCellContentImageBlock> &image_blocks
            = cell_content.image_blocks;

        const std::vector<OriPageStructuredTableCellContentTextBlock> &text_blocks
            = cell_content.text_blocks;

        if (image_blocks.empty() && text_blocks.empty()) {
            LOG("Function name: " << __func__
                << ", image_blocks size: " << image_blocks.size()
                << ", text_blocks size: " << text_blocks.size());
            return ERROR_CODE_TYPE::ERR_INTERNAL_FAILED;
        }

        result_link = LinkTableCellAndPageContent(image_blocks, text_blocks, page_content, &pub_cell);
        if (result_link != ERROR_CODE_TYPE::ERR_OK)
            return ERROR_CODE_TYPE::ERR_INTERNAL_FAILED;
        pub_table->cells[index] = std::move(pub_cell);
    }
    return result_link;
}

 int TextInParserEngine::ConvertDocPageTables(const OriPage& page,
        intsig::textin::parser_engine::Page * pub_page) {
    LOG("Function name: " << __func__);
    const OriPageStructured &structured = page.structured;
    const OriPageContent &content = page.content;

    const std::vector<OriPageStructuredTable> &table_structs = structured.tables;

    if (table_structs.empty()) {
        LOG("Function name: " << __func__
            << ", table structures size: " << table_structs.size());
        return ERROR_CODE_TYPE::ERR_OK;
    }

    if (table_structs.size()
            < intsig::textin::parser_engine::Page::kMinPageTableCount ||
        table_structs.size()
            > intsig::textin::parser_engine::Page::kMaxPageTableCount) {
        LOG("current table count: " << table_structs.size()
        << ", min: " << intsig::textin::parser_engine::Page::kMinPageTableCount
        << ", max: " << intsig::textin::parser_engine::Page::kMaxPageTableCount);
        return ERROR_CODE_TYPE::ERR_INTERNAL_FAILED;
    }

    pub_page->table_size = table_structs.size();
    pub_page->page_tables = (intsig::textin::parser_engine::Table*)malloc(
        pub_page->table_size * sizeof(intsig::textin::parser_engine::Table));

    int result_convter = ERROR_CODE_TYPE::ERR_OK;
    for (size_t index = 0; index < table_structs.size(); index++) {
        intsig::textin::parser_engine::Table pub_table;
        const auto& table_struct = table_structs[index];
        ASSIGN_POS(pub_table, table_struct, TRANSFER_MAPPING);

        pub_table.rows = table_struct.rows;
        pub_table.cols = table_struct.cols;
        pub_table.sub_type = table_struct.sub_type.c_str();
        pub_table.is_continue = table_struct.continue_table;

        if (!table_struct.columns_width.empty()) {
            pub_table.colums_width_size = table_struct.columns_width.size();
            pub_table.colums_width = (int*)malloc(
                pub_table.colums_width_size * sizeof(int));
            std::copy(table_struct.columns_width.begin(),
                table_struct.columns_width.end(), pub_table.colums_width);
        }

        if (!table_struct.rows_height.empty()) {
            pub_table.rows_height_size = table_struct.rows_height.size();
            pub_table.rows_height = (int*)malloc(
                pub_table.rows_height_size * sizeof(int));
            std::copy(table_struct.rows_height.begin(),
                  table_struct.rows_height.end(), pub_table.rows_height);
        }

        result_convter = ConverteCells(table_struct, content, &pub_table);
        if (result_convter != ERROR_CODE_TYPE::ERR_OK) return result_convter;
        pub_page->page_tables[index] = pub_table;
    }
    return result_convter;
}

int TextInParserEngine::ConvertDocPages() {
    LOG("Function name: " << __func__);
    std::vector<OriPage> &pages = res->result.pages;
    if (pages.empty()) {
        LOG("Function name: " << __func__ << ", Origin parsed pages is empty");
        return ERROR_CODE_TYPE::ERR_EMPTY_PAGE;
    }
    tmp_pub_doc->page_size = pages.size();

    tmp_pub_doc->pages = (intsig::textin::parser_engine::Page*)malloc(
                 pages.size() * sizeof(intsig::textin::parser_engine::Page));
    if (!tmp_pub_doc->pages) {
        std::cerr << "MEM ALLOC FAIL!!!\n";
        return ERROR_CODE_TYPE::ERR_INTERNAL_FAILED;
    }

    int result_convter = ERROR_CODE_TYPE::ERR_OK;
    for (size_t index = 0; index < pages.size(); index++) {
        const auto& page = pages[index];
        intsig::textin::parser_engine::Page pub_page;
        pub_page.page_id = page.page_id;
        pub_page.page_durations = page.durations;

        pub_page.page_status = page.status.c_str();
        pub_page.page_image_id = page.image_id.c_str();
        result_convter = ConvertDocPageContent(page, &pub_page);

        if (result_convter != ERROR_CODE_TYPE::ERR_OK) return result_convter;
        tmp_pub_doc->pages[index] = std::move(pub_page); // insert new paga
    }
    return result_convter;
}

int TextInParserEngine::StartConvertToPubDoc(const json& j) {
    LOG("Function name: " << __func__);

    // print_struct(*res);
     if (res) return ERROR_CODE_TYPE::ERR_INTERNAL_FAILED;
    // Gen original json struct
    res = std::make_shared<APIResponse>(j.get<APIResponse>());

    if (!tmp_pub_doc) return ERROR_CODE_TYPE::ERR_INTERNAL_FAILED;
    ConvertDocMetaData();
    ConvertDocMetrics();

    // int parse_page_size = res->output.result.pages.size();
    // if (parse_page_size > intsig::textin::parser_engine::Document::kMaxDocumentPages ||
    //     parse_page_size > intsig::textin::parser_engine::Document::kMinDocumentPages) {
    //     LOG("the total number of current pages is greater than the set value"
    //         << ", max: " << intsig::textin::parser_engine::Document::kMaxDocumentPages
    //         << ", min: " << intsig::textin::parser_engine::Document::kMinDocumentPages);
    //     return ERROR_CODE_TYPE::ERR_INTERNAL_FAILED;
    // }

    return ConvertDocPages();
}

void TextInParserEngine::StartInit() {
    if (!tmp_pub_doc)
        tmp_pub_doc = std::make_shared<intsig::textin::parser_engine::Document>();
    // if (!res) res = std::make_shared<APIResponse>();
}

int TextInParserEngine::parse(const char * raw) {
    LOG("Function name: " << __func__);
    StartInit();

    int paser_result = ERROR_CODE_TYPE::ERR_OK;
    try {
        json j = json::parse(raw);
        paser_result = StartConvertToPubDoc(j);
    } catch (const json::parse_error& e) {
        std::cerr << "Error: " << std::string(e.what()) << std::endl;
    }

    if (res) res.reset();

    // print_document(tmp_pub_doc.get(), 0);
    return paser_result;
}

int TextInParserEngine::parseFile(const char * file_path) {
    LOG("Function name: " << __func__);
    StartInit();

    int parse_result = ERROR_CODE_TYPE::ERR_OK;
    try {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << file_path << std::endl;
            return ERROR_CODE_TYPE::ERR_FILE_NOT_FOUND;
        }

        json j;
        file >> j;

        parse_result = StartConvertToPubDoc(j);
    } catch (const json::parse_error& e) {
        std::cerr << "Error: " << std::string(e.what()) << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << std::string(e.what()) << std::endl;
    }

    if (res) res.reset();

    // print_document(tmp_pub_doc.get(), 0);
    return parse_result;
}

void release_image_content(intsig::textin::parser_engine::ImageContent * content) {
    if (!content) return;
    LOG("Function name: " << __func__);
    if (content->size) free(content->size);
}

void release_image_block(intsig::textin::parser_engine::Imageblock * block) {
    if (!block) return;
    LOG("Function name: " << __func__);
    if (block->i_contents) {
        for (int i = 0; i < block->content_size; ++i) {
            release_image_content(&block->i_contents[i]);
        }
        free(block->i_contents);
    }
}

void release_text_content(intsig::textin::parser_engine::TextContent* content) {
    if (!content) return;
    LOG("Function name: " << __func__);
    if (content->char_pos) {
        free(content->char_pos);
        content->char_pos = nullptr;
    }
    if (content->char_cand) {
        free(content->char_cand);
        content->char_cand = nullptr;
    }
    if (content->char_cand_size > 0) {

    }
    if (content->char_cand_score) {
        free(content->char_cand_score);
        content->char_cand_score = nullptr;
    }
}

void release_text_block(intsig::textin::parser_engine::Textblock* block) {
    if (!block) return;
    LOG("Function name: " << __func__);
    if (block->t_contents) {
        for (int i = 0; i < block->content_size; ++i) {
            release_text_content(&block->t_contents[i]);
        }
        free(block->t_contents);
    }
}

void release_cell(intsig::textin::parser_engine::Cell* cell) {
    if (!cell) return;
    LOG("Function name: " << __func__);
    if (cell->t_blocks) {
        for (int i = 0; i < cell->t_block_size; ++i) {
            release_text_block(&cell->t_blocks[i]);
        }
        free(cell->t_blocks);
    }
    if (cell->i_blocks) {
        for (int i = 0; i < cell->i_block_size; ++i) {
            release_image_block(&cell->i_blocks[i]);
        }
        free(cell->i_blocks);
    }
}

void release_table(intsig::textin::parser_engine::Table* table) {
    if (!table) return;
    LOG("Function name: " << __func__);
    if (table->colums_width) free(table->colums_width);
    if (table->rows_height) free(table->rows_height);
    if (table->cells) {
        for (int i = 0; i < table->cell_size; ++i) {
            release_cell(&table->cells[i]);
        }
        free(table->cells);
    }
}

void release_page(intsig::textin::parser_engine::Page* page) {
    if (!page) return;
    LOG("Function name: " << __func__);
    if (page->page_tables) {
        for (int i = 0; i < page->table_size; ++i) {
            release_table(&page->page_tables[i]);
        }
        free(page->page_tables);
    }
}

ITextInParserEngine* ITextInParserEngine::createAndStartTextInParserEngine() {
    LOG("Function name: " << __func__);
    return TextInParserEngine::createAndStartTextInParserEngine();
}

int TextInParserEngine::release()  {
    LOG("Function name: " << __func__);
    if (!tmp_pub_doc) return ERROR_CODE_TYPE::ERR_INTERNAL_FAILED;

    if (tmp_pub_doc->pages) {
        for (int i = 0; i < tmp_pub_doc->page_size; ++i) {
            release_page(&tmp_pub_doc->pages[i]);
        }
        free(tmp_pub_doc->pages);
    }
    tmp_pub_doc.reset();
    tmp_pub_doc = nullptr;
    return ERROR_CODE_TYPE::ERR_OK;
}

int TextInParserEngine::findTables(int page_id,
        intsig::textin::parser_engine::Table* buffer, int size) {
    LOG("Function name: " << __func__);
    if (!tmp_pub_doc) return ERROR_CODE_TYPE::ERR_BEFORE_PARSE;
    if (!buffer) return ERROR_CODE_TYPE::ERR_NULL_BUFFER;
    if (page_id + 1 > tmp_pub_doc->page_size || page_id < 0)
        return ERROR_CODE_TYPE::ERR_ID_OUT_RANGE;

    const intsig::textin::parser_engine::Page& current_page
        = tmp_pub_doc->pages[page_id];

    if (current_page.table_size <= 0) return ERROR_CODE_TYPE::ERR_EMPTY_TABLE;

    int copy_size = current_page.table_size < size ? current_page.table_size : size;
    for (int i = 0; i < copy_size; ++i) {
        buffer[i] = current_page.page_tables[i];
    }

    return ERROR_CODE_TYPE::ERR_OK;
}

int TextInParserEngine::getPageSize(int * page_size) {
    LOG("Function name: " << __func__);
    if (!tmp_pub_doc) return ERROR_CODE_TYPE::ERR_BEFORE_PARSE;
    if (!page_size) return ERROR_CODE_TYPE::ERR_FAILED;
    *page_size = tmp_pub_doc->page_size;
    return ERROR_CODE_TYPE::ERR_OK;
}

int TextInParserEngine::getPageTablesSize(int page_id, int * table_size) {
    LOG("Function name: " << __func__);
    if (!tmp_pub_doc) return ERROR_CODE_TYPE::ERR_BEFORE_PARSE;

    if (page_id + 1 > tmp_pub_doc->page_size || page_id < 0)
        return ERROR_CODE_TYPE::ERR_ID_OUT_RANGE;

    if (!table_size) return ERROR_CODE_TYPE::ERR_FAILED;
    *table_size = tmp_pub_doc->pages[page_id].table_size;
    return ERROR_CODE_TYPE::ERR_OK;
}

 intsig::textin::parser_engine::Document TextInParserEngine::getDocument() {
    LOG("Function name: " << __func__);
    return *tmp_pub_doc;
 }

}
}
}
