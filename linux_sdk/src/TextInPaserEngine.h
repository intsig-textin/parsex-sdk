// Copyright (c) 2024 Intsig.net. All rights reserved

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <memory>
#include <unordered_map>

#include "../include/ITextInParserEngine.h"
#include "../include/ITextInLinuxSdkCommon.h"

namespace intsig {
namespace textin {
namespace parser_engine{

using json = nlohmann::json;
// original json struct page structured & content

/////page structured table cell content detail

struct OriPageStructuredTableCellContentTextBlock {
  std::vector<int> pos;
  std::vector<int> content_ids;
  std::string sub_type;
  bool is_continue = false;
};

struct OriPageStructuredTableCellContentImageBlock {
  std::vector<int> pos;
  std::vector<int> line_ids;
  int zorder = 0;
  std::vector<int> content_ids;
};

//////////page structured table cell detail

struct OriPageStructuredTableCellContent {
  std::vector<OriPageStructuredTableCellContentImageBlock> image_blocks;
  std::vector<OriPageStructuredTableCellContentTextBlock> text_blocks;
};

/////////page content image data detail

struct OriPageContentImageDataImageData {
  std::string base64;
  std::vector<int> region;
  std::string path;
};

/////////////////////page structured table detail

struct OriPageStructuredTableCell {
  int row = 0;
  int col = 0;
  int row_span = 1;
  int col_span = 1;
  std::vector<int> pos;
  OriPageStructuredTableCellContent content;
};

/////////////////////page content image detail

struct OriPageContentImageData {
  OriPageContentImageDataImageData image_data;
};

////////////////////////////////page readable detail
// struct OriPageReadableParagraph {

// };

// struct OriPageReadableTableText {

// };

////////////////////////////////page content detail

struct OriPageContentTextline {
  int id = -1;
  std::string text;
  int angle = 0;
  std::vector<int> pos;
  std::string sub_type;
  int direction = 0;
  float score = 0.0f;
  std::vector<std::vector<int>> char_pos;
  std::vector<std::vector<std::string>> char_cand;
  std::vector<std::vector<float>> char_cand_score;
};

struct OriPageContentImage {
  int id = -1;
  std::vector<int> pos;
  std::string sub_type;
  std::string stamp_type;
  std::string stamp_shape;
  std::string stamp_color;
  std::vector<int> size;
  OriPageContentImageData data;
};

////////////////////////////////page structured detail
// struct OriPageStructuredHaeder {

// };

// struct OriPageStructuredFooter {

// };

// struct OriPageStructuredImageblock {

// };


struct OriPageStructuredTable {
  std::string sub_type;
  std::vector<int> pos;
  int rows = 0;
  int cols = 0;
  std::vector<int> columns_width;
  std::vector<int> rows_height;
  bool continue_table = false;
  std::vector<OriPageStructuredTableCell> cells;
};

struct OriPageStructuredTextblock {
  std::string sub_type;
  std::vector<int> pos;
  std::vector<int> contents;
  bool is_continue = false;
};

////////////////////////////////page detail

struct OriPageStructured {
  // std::vector<OriPageStructuredTextblock> textblocks;
  std::vector<OriPageStructuredTable> tables;
  // std::vector<OriPageStructuredImageblock> imageblocks;
  // std::vector<OriPageStructuredFooter> footers;
  // std::vector<OriPageStructuredHaeder> headers;
};

struct OriPageContent {
  std::vector<OriPageContentTextline> text_lines;
  std::vector<OriPageContentImage> images;
};

// struct OriPageReadable {
//   std::vector<OriPageReadableTableText> table_texts;
//   std::vector<OriPageReadableParagraph> paragraphs;
// };

struct OriPageImage {
  std::string base64;
  std::vector<int> region;
  std::string path;
};

struct OriPage {
  std::string status;
  std::string image_id;
  int page_id = -1;
  float durations = 0.0f;
  int width = 0;
  int height = 0;
  int angle = 0;
  int num = 0;
  OriPageImage image;
  OriPageContent content;
  // OriPageReadable readable;
  OriPageStructured structured;
};

struct OriResult {
  int src_page_count = 0;
  std::string markdown;
  std::vector<OriPage> pages;
};

struct OriMetrics {
  std::string document_type;
  int total_page_number = 0;
  int valid_page_number = 0;
  int paragraph_number = 0;
  int character_number = 0;
};

struct APIResponse {
  int code = 0;
  int duration = 0;
  std::string message;
  OriResult result;
  OriMetrics metrics;
  std::string version;
};

struct TRANSFER_MAPPING {
    static const int X1 = 0;
    static const int Y1 = 1;
    static const int X2 = 2;
    static const int Y2 = 3;
    static const int X3 = 4;
    static const int Y3 = 5;
    static const int X4 = 6;
    static const int Y4 = 7;
};

class TextInParserEngine :
  public intsig::textin::parser_engine::ITextInParserEngine {

public:
 int parse(const char * raw) override;

 int parseFile(const char* path) override;

 int findTables(int page_id,
  intsig::textin::parser_engine::Table* buffer, int size) override;

 intsig::textin::parser_engine::Document getDocument() override;

 int getPageTablesSize(int page_id, int * table_size) override;

 int getPageSize(int * page_size) override;

 int release() override;

 TextInParserEngine() {}
 virtual ~TextInParserEngine() {}

 static ITextInParserEngine* createAndStartTextInParserEngine() {
      return new TextInParserEngine();
 }

private:
 std::shared_ptr<intsig::textin::parser_engine::Document> tmp_pub_doc = nullptr;

private:
 std::shared_ptr<APIResponse> res = nullptr;

private:
 int StartConvertToPubDoc(const json& j);
 void StartInit();

 void ConvertDocMetaData();
 void ConvertDocMetrics();
 int ConvertDocPages();

 //(NOTE)Jingshuwei:Currently, only table extraction is supported
 int ConvertDocPageContent(const OriPage& page,
    intsig::textin::parser_engine::Page * pub_page);
 int ConvertDocPageTables(const OriPage& page,
    intsig::textin::parser_engine::Page * pub_page);

 int ConverteCells(
    const OriPageStructuredTable& table_struct,
    const OriPageContent &page_content,
    intsig::textin::parser_engine::Table * pub_table);

 int LinkTableCellAndPageContent(
    const std::vector<OriPageStructuredTableCellContentImageBlock> &image_blocks,
    const std::vector<OriPageStructuredTableCellContentTextBlock> &text_blocks,
    const OriPageContent &page_contents,
    intsig::textin::parser_engine::Cell * pub_cell);

  bool LinkCellPartImage(const OriPageStructuredTableCellContentImageBlock& image_block,
    const std::unordered_map<int, const OriPageContentImage*> &image_map,
    int cell_i_block_index,
    intsig::textin::parser_engine::Cell *cell);

  bool LinkCellPartText(const OriPageStructuredTableCellContentTextBlock& text_blocks,
    const std::unordered_map<int, const OriPageContentTextline*> &text_line_map,
    int cell_t_block_index,
    intsig::textin::parser_engine::Cell *cell);
};

}
}
}
