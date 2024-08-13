#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>
#include "../include/ITextInParserEngine.h"

namespace py = pybind11;
using namespace intsig::textin::parser_engine;

PYBIND11_MODULE(textin_parser, m) {
    py::class_<Pos>(m, "Pos")
        .def(py::init<>())
        .def(py::init<int, int, int, int, int, int, int, int>())
        .def_readwrite("x1", &Pos::x1)
        .def_readwrite("y1", &Pos::y1)
        .def_readwrite("x2", &Pos::x2)
        .def_readwrite("y2", &Pos::y2)
        .def_readwrite("x3", &Pos::x3)
        .def_readwrite("y3", &Pos::y3)
        .def_readwrite("x4", &Pos::x4)
        .def_readwrite("y4", &Pos::y4);

    py::class_<ImageContent>(m, "ImageContent")
        .def(py::init<>())
        .def_readwrite("stamp_type", &ImageContent::stamp_type)
        .def_readwrite("stamp_color", &ImageContent::stamp_color)
        .def_readwrite("sub_type", &ImageContent::sub_type)
        .def_readwrite("pos", &ImageContent::pos)
        .def_readwrite("base64", &ImageContent::base64)
        .def_readwrite("path", &ImageContent::path)
        // Convert size array to Python list
        .def_property_readonly("size", [](const ImageContent &self) {
            std::vector<int> size_list;
            for (int i = 0; i < self.size_size; ++i) {
                size_list.push_back(self.size[i]);
            }
            return size_list;
        })
        .def_readwrite("size_size", &ImageContent::size_size);

    py::class_<Imageblock>(m, "Imageblock")
        .def(py::init<>())
        .def_readwrite("pos", &Imageblock::pos)
        .def_readwrite("zorder", &Imageblock::zorder)
        .def_readwrite("sub_type", &Imageblock::sub_type)
        // Convert i_contents array to Python list
        .def_property_readonly("i_contents", [](const Imageblock &self) {
            std::vector<ImageContent> i_contents_list;
            for (int i = 0; i < self.content_size; ++i) {
                i_contents_list.push_back(self.i_contents[i]);
            }
            return i_contents_list;
        })
        .def_readwrite("content_size", &Imageblock::content_size);

    py::class_<TextContent>(m, "TextContent")
        .def(py::init<>())
        .def_readwrite("text", &TextContent::text)
        .def_readwrite("direction", &TextContent::direction)
        .def_readwrite("score", &TextContent::score)
        .def_readwrite("angle", &TextContent::angle)
        // Convert char_pos array to Python list
        .def_property_readonly("char_pos", [](const TextContent &self) {
            std::vector<Pos> pos_list;
            for (int i = 0; i < self.char_pos_size; ++i) {
                pos_list.push_back(self.char_pos[i]);
            }
            return pos_list;
        })
        .def_readwrite("char_pos_size", &TextContent::char_pos_size)
        // Convert char_cand array to Python list
        .def_property_readonly("char_cand", [](const TextContent &self) {
            std::vector<TextContent::Cand> cand_list;
            for (int i = 0; i < self.char_cand_size; ++i) {
                cand_list.push_back(self.char_cand[i]);
            }
            return cand_list;
        })
        .def_readwrite("char_cand_size", &TextContent::char_cand_size)
        // Convert char_cand_score array to Python list
        .def_property_readonly("char_cand_score", [](const TextContent &self) {
            std::vector<TextContent::CandScore> score_list;
            for (int i = 0; i < self.char_cand_score_size; ++i) {
                score_list.push_back(self.char_cand_score[i]);
            }
            return score_list;
        })
        .def_readwrite("char_cand_score_size", &TextContent::char_cand_score_size);

    py::class_<TextContent::Cand>(m, "Cand")
        .def(py::init<>())
        .def_readwrite("cand_str", &TextContent::Cand::cand_str);

    py::class_<TextContent::CandScore>(m, "CandScore")
        .def(py::init<>())
        .def_readwrite("score", &TextContent::CandScore::score);

    py::class_<Textblock>(m, "Textblock")
        .def(py::init<>())
        .def_readwrite("pos", &Textblock::pos)
        .def_readwrite("sub_type", &Textblock::sub_type)
        .def_readwrite("is_continue", &Textblock::is_continue)
        // Convert t_contents array to Python list
        .def_property_readonly("t_contents", [](const Textblock &self) {
            std::vector<TextContent> t_contents_list;
            for (int i = 0; i < self.content_size; ++i) {
                t_contents_list.push_back(self.t_contents[i]);
            }
            return t_contents_list;
        })
        .def_readwrite("content_size", &Textblock::content_size);

    py::class_<Cell>(m, "Cell")
        .def(py::init<>())
        .def_readwrite("pos", &Cell::pos)
        .def_readwrite("row", &Cell::row)
        .def_readwrite("col", &Cell::col)
        // Convert t_blocks array to Python list
        .def_property_readonly("t_blocks", [](const Cell &self) {
            std::vector<Textblock> t_blocks_list;
            for (int i = 0; i < self.t_block_size; ++i) {
                t_blocks_list.push_back(self.t_blocks[i]);
            }
            return t_blocks_list;
        })
        .def_readwrite("t_block_size", &Cell::t_block_size)
        // Convert i_blocks array to Python list
        .def_property_readonly("i_blocks", [](const Cell &self) {
            std::vector<Imageblock> i_blocks_list;
            for (int i = 0; i < self.i_block_size; ++i) {
                i_blocks_list.push_back(self.i_blocks[i]);
            }
            return i_blocks_list;
        })
        .def_readwrite("i_block_size", &Cell::i_block_size);

    py::class_<Table>(m, "Table")
        .def(py::init<>())
        .def_readwrite("sub_type", &Table::sub_type)
        .def_readwrite("pos", &Table::pos)
        .def_readwrite("rows", &Table::rows)
        .def_readwrite("cols", &Table::cols)
        // Convert colums_width array to Python list
        .def_property_readonly("colums_width", [](const Table &self) {
            std::vector<int> col_width_list;
            for (int i = 0; i < self.colums_width_size; ++i) {
                col_width_list.push_back(self.colums_width[i]);
            }
            return col_width_list;
        })
        .def_readwrite("colums_width_size", &Table::colums_width_size)
        // Convert rows_height array to Python list
        .def_property_readonly("rows_height", [](const Table &self) {
            std::vector<int> row_height_list;
            for (int i = 0; i < self.rows_height_size; ++i) {
                row_height_list.push_back(self.rows_height[i]);
            }
            return row_height_list;
        })
        .def_readwrite("rows_height_size", &Table::rows_height_size)
        // Convert cells array to Python list
        .def_property_readonly("cells", [](const Table &self) {
            std::vector<Cell> cells_list;
            for (int i = 0; i < self.cell_size; ++i) {
                cells_list.push_back(self.cells[i]);
            }
            return cells_list;
        })
        .def_readwrite("cell_size", &Table::cell_size)
        .def_readwrite("is_continue", &Table::is_continue);

    py::class_<Page>(m, "Page")
        .def(py::init<>())
        .def_readwrite("page_id", &Page::page_id)
        .def_readwrite("page_status", &Page::page_status)
        .def_readwrite("page_image_id", &Page::page_image_id)
        .def_readwrite("page_durations", &Page::page_durations)
        // Convert page_tables array to Python list
        .def_property_readonly("page_tables", [](const Page &self) {
            std::vector<Table> tables_list;
            for (int i = 0; i < self.table_size; ++i) {
                tables_list.push_back(self.page_tables[i]);
            }
            return tables_list;
        })
        .def_readwrite("table_size", &Page::table_size);

    py::class_<MetaData>(m, "MetaData")
        .def(py::init<>())
        .def_readwrite("version", &MetaData::version)
        .def_readwrite("markdown", &MetaData::markdown)
        .def_readwrite("duration", &MetaData::duration);

    py::class_<Metrics>(m, "Metrics")
        .def(py::init<>())
        .def_readwrite("page_total", &Metrics::page_total)
        .def_readwrite("page_valid", &Metrics::page_valid)
        .def_readwrite("paragraph_num", &Metrics::paragraph_num)
        .def_readwrite("char_num", &Metrics::char_num)
        .def_readwrite("document_type", &Metrics::document_type);

    py::class_<Document>(m, "Document")
        .def(py::init<>())
        .def_property_readonly("pages", [](const Document &self) {
            std::vector<Page> pages_list;
            for (int i = 0; i < self.page_size; ++i) {
                pages_list.push_back(self.pages[i]);
            }
            return pages_list;
        })
        .def_readwrite("page_size", &Document::page_size)
        .def_readwrite("metrics", &Document::metrics)
        .def_readwrite("metadata", &Document::metadata)
        .def_readwrite("encrypted_state", &Document::encrypted_state);

    py::class_<ITextInParserEngine>(m, "ITextInParserEngine")
        .def_static("createAndStartTextInParserEngine", &ITextInParserEngine::createAndStartTextInParserEngine)
        .def("parse", &ITextInParserEngine::parse)
        .def("parseFile", &ITextInParserEngine::parseFile)
        .def("release", &ITextInParserEngine::release)
        .def("getPageSize", [](ITextInParserEngine &self) {
            int page_size = 0;
            int result = self.getPageSize(&page_size);
            return std::make_tuple(result, page_size);
        })
        .def("getPageTablesSize", [](ITextInParserEngine &self, int page_id) {
            int table_size = 0;
            int result = self.getPageTablesSize(page_id, &table_size);
            return std::make_tuple(result, table_size);
        })
        .def("findTables", [](ITextInParserEngine &self, int page_id, int table_size) {
            std::vector<Table> tables(table_size);
            int result = self.findTables(page_id, tables.data(), tables.size());
            return std::make_tuple(result, tables);
        })
        .def("getDocument", &ITextInParserEngine::getDocument);
}
