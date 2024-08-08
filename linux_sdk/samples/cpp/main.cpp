#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <algorithm> // std::min
#include <cstdlib>   // std::system
#include <string>    // std::string
#include "../../include/ITextInParserEngine.h"

namespace fs = std::filesystem;
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

void processFile(const std::string& filePath, ITextInParserEngine* engine) {
    // std::ifstream file(filePath);
    // if (!file.is_open()) {
    //     std::cerr << "Failed to open file: " << filePath << std::endl;
    //     return;
    // }

    // std::string jsonContent((std::istreambuf_iterator<char>(file)),
    //                          std::istreambuf_iterator<char>());
    // file.close();
    // int result = engine->parse(jsonContent.c_str());
    // if (result != ERROR_CODE_TYPE::ERR_OK) {
    //     std::cerr << "Failed to parse file: " << filePath << " with error code: " << result << std::endl;
    //     return;
    // }

    int result = engine->parseFile(filePath.c_str());
    if (result != ERROR_CODE_TYPE::ERR_OK) {
        std::cerr << "Failed to parse file: " << filePath << " with error code: " << result << std::endl;
        return;
    }

    Document document = engine->getDocument();
    // Print and verify document metrics and metadata
    std::cout << "Document Version: " << document.metadata.version << std::endl;
    std::cout << "Document Duration: " << document.metadata.duration << std::endl;
    std::cout << "Document Type: " << document.metrics.document_type << std::endl;
    std::cout << "Document Page Total: " << document.metrics.page_total << std::endl;
    std::cout << "Document MarkDown: " << document.metadata.markdown << std::endl;

    int pageSize = 0;
    engine->getPageSize(&pageSize);
    std::cout << "Document Page Size: " << pageSize << std::endl;

    for (int i = 0; i < pageSize; ++i) {
        const auto &current_page = document.pages[i];
        std::cout << "Page [" << i << "]"<< std::endl;
        std::cout << "Page Status: " << current_page.page_status << std::endl;
        std::cout << "Page Image Id: " << current_page.page_image_id << std::endl;
        std::cout << "Page Id: " << current_page.page_id << std::endl;
        std::cout << "Page Durations: " << current_page.page_durations << std::endl;
    }
    std::cout << std::endl;
    for (int i = 0; i < pageSize; ++i) {
        int tableSize = 0;
        engine->getPageTablesSize(i, &tableSize);
        std::cout << "Page [" << i << "]" << " Table Size: " << tableSize << std::endl;

        if (tableSize == 0) continue;

        std::vector<Table> tables(tableSize);
        result = engine->findTables(i, tables.data(), tableSize);
        if (result != ERROR_CODE_TYPE::ERR_OK) {
            std::cerr << "Failed to find tables for page: " << i << " with error code: " << result << std::endl;
            continue;
        }

        for (size_t i = 0; i < std::min(tables.size(), static_cast<size_t>(kMaxPrintTable)); i++) {
            std::cout << "Table [" << i << "]"<< std::endl;
            std::cout << "Table Sub Type: " << tables[i].sub_type << std::endl;
            std::cout << "Table Rows: " << tables[i].rows << std::endl;
            std::cout << "Table Cols: " << tables[i].cols << std::endl;

            std::cout << "Table Colums_width: ";
            for (int j = 0; j < tables[i].colums_width_size; ++j) {
                std::cout << "width[" << j << "]: " << tables[i].colums_width[j] << " ";
            }
            std::cout << std::endl;

            std::cout << "Table Rows_height: ";
            for (int j = 0; j < tables[i].rows_height_size; ++j) {
                std::cout << "height[" << j << "]: " << tables[i].rows_height[j] << " ";
            }
            std::cout << std::endl;

            std::cout << "Table Continue: " << (tables[i].is_continue ? "True" : "False") << std::endl;

            std::cout << "Table Cells: " << std::endl;
            for (size_t c = 0; c < std::min(static_cast<size_t>(tables[i].cell_size), static_cast<size_t>(kMaxPrintCell)); ++c) {
                std::cout << "Cell[" << c << "]"<< std::endl;
                std::cout << "Cell[" << c << "]-"<< "Pos: ";
                print_pos( tables[i].cells[c].pos);
                std::cout << std::endl;
                std::cout << "Cell[" << c << "]-"<< "Row:" << tables[i].cells[c].row << std::endl;
                std::cout << "Cell[" << c << "]-"<< "Col:" << tables[i].cells[c].col << std::endl;
                // process text
                for (int t = 0; t < tables[i].cells[c].t_block_size; ++t) {
                    std::cout << "TextBlock[" << t << "]"<< std::endl;
                    std::cout << "TextBlock[" << t << "]-"<< "Continue: "
                        << (tables[i].cells[c].t_blocks[t].is_continue ? "True" : "False") << std::endl;

                    std::cout << "TextBlock[" << t << "]-"<< "Subtype: "
                        << tables[i].cells[c].t_blocks[t].sub_type << std::endl;

                    std::cout << "TextBlock[" << t << "]-"<< "AllText:";
                    for (int g = 0; g < tables[i].cells[c].t_blocks[t].content_size; g++) {
                        std::cout << tables[i].cells[c].t_blocks[t].t_contents[g].text;
                    }
                    std::cout << std::endl;

                    std::cout << "TextBlock[" << t << "]-" "All Direction: ";
                    for (int g = 0; g < tables[i].cells[c].t_blocks[t].content_size; g++) {
                       std::cout << tables[i].cells[c].t_blocks[t].t_contents[g].direction << " ";
                    }
                    std::cout << std::endl;

                    std::cout << "TextBlock[" << t << "]-" "All Angle: ";
                    for (int g = 0; g < tables[i].cells[c].t_blocks[t].content_size; g++) {
                       std::cout << tables[i].cells[c].t_blocks[t].t_contents[g].angle << " ";
                    }
                    std::cout << std::endl;

                    std::cout << "TextBlock[" << t << "]-" "All Score: ";
                    for (int g = 0; g < tables[i].cells[c].t_blocks[t].content_size; g++) {
                       std::cout << tables[i].cells[c].t_blocks[t].t_contents[g].score << " ";
                    }
                    std::cout << std::endl;

                    std::cout << "TextBlock[" << t << "]-" "All Char Pos: " << std::endl;
                    for (int g = 0; g < tables[i].cells[c].t_blocks[t].content_size; g++) {
                        for (int p = 0; p < tables[i].cells[c].t_blocks[t].t_contents[g].char_pos_size; ++p) {
                            std::cout << "char[" << p << "] pos: ";
                            print_pos(tables[i].cells[c].t_blocks[t].t_contents[g].char_pos[p]);
                            std::cout << std::endl;
                        }
                    }
                    std::cout << std::endl;
                }
                std::cout << std::endl;

                // process image
                for (int t = 0; t < tables[i].cells[c].i_block_size; ++t) {
                    std::cout << "ImageBlock[" << t << "]"<< std::endl;
                    std::cout << "ImageBlock[" << t << "]-"<< "Subtype: "
                        << tables[i].cells[c].i_blocks[t].sub_type << std::endl;
                    std::cout << "ImageBlock[" << t << "]-"<< "Zorder: "
                        << tables[i].cells[c].i_blocks[t].zorder << std::endl;
                    std::cout << "ImageBlock[" << t << "]-"<< "Pos: ";
                        print_pos(tables[i].cells[c].i_blocks[t].pos);
                        std::cout << std::endl;
                }
            }
            std::cout << std::endl;
        }
    }

    engine->release();
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << std::endl;
    }

    // std::string appId = argv[1];
    // std::string secretCode = argv[2];

    ITextInParserEngine* engine = ITextInParserEngine::createAndStartTextInParserEngine();
    if (!engine) {
        std::cerr << "Failed to create parser engine." << std::endl;
        return 1;
    }

    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path parentPath = currentPath.parent_path();
    std::filesystem::path fileDir = parentPath / "file";
    std::filesystem::path resourceDir = parentPath / "resource";
    std::filesystem::path outputDir = parentPath / "output";
    std::filesystem::path logFilePath = outputDir / "log.txt";

    if (!std::filesystem::exists(resourceDir)) {
        std::filesystem::create_directories(resourceDir);
    }
    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directories(outputDir);
    }

    // for (const auto& entry : fs::directory_iterator(resourceDir)) {
    //     std::filesystem::remove_all(entry.path());
    // }

    for (const auto& entry : fs::directory_iterator(outputDir)) {
        std::filesystem::remove_all(entry.path());
    }

    std::ofstream logFile(logFilePath);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file: " << logFilePath << std::endl;
        return 1;
    }

    std::streambuf* coutBuf = std::cout.rdbuf();
    std::streambuf* cerrBuf = std::cerr.rdbuf();
    std::cout.rdbuf(logFile.rdbuf());
    std::cerr.rdbuf(logFile.rdbuf());

    // for (const auto& entry : fs::directory_iterator(fileDir)) {
    //     if (entry.is_regular_file()) {
    //         std::string inputFilePath = entry.path().string();
    //         std::string outputFilePath = (resourceDir / ("result_" + entry.path().filename().string() + ".json")).string();

    //         std::string curlCommand = "curl --location --request POST 'https://api.textin.com/ai/service/v1/x_to_markdown?md_detail=1' "
    //                                   "--header 'x-ti-app-id: " + appId + "' "
    //                                   "--header 'x-ti-secret-code: " + secretCode + "' "
    //                                   "--data-binary '@" + inputFilePath + "' "
    //                                   "--output " + outputFilePath;

    //         int ret = std::system(curlCommand.c_str());
    //         if (ret != 0) {
    //             std::cerr << "Failed to download JSON for file: " << inputFilePath << std::endl;
    //             continue;
    //         }

    //         std::cout << "Downloaded JSON for file: " << inputFilePath << std::endl;
    //     }
    // }

    for (const auto& entry : fs::directory_iterator(resourceDir)) {
        if (entry.path().extension() == ".json") {
            std::cout << "---------------Processing file: " << entry.path().string() << "---------------" << std::endl;
            processFile(entry.path().string(), engine);
            std::cout << "---------------Processing Complete ---------------" << std::endl;
        }
    }

    delete engine;

    std::cout.rdbuf(coutBuf);
    std::cerr.rdbuf(cerrBuf);
    logFile.close();

    return 0;
}
