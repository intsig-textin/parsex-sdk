#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <algorithm> // std::min
#include <cstdlib>   // std::system
#include <string>    // std::string
#include "../../include/ITextInParserEngine.h"
#include "../base/parser_print_helper.h"

namespace fs = std::filesystem;
using namespace intsig::textin::parser_engine;
using namespace intsig::textin::linuxsdk;

void processFile(const std::string& filePath, ITextInParserEngine* engine) {
    int result = engine->parseFile(filePath.c_str());
    if (result != ERROR_CODE_TYPE::ERR_OK) {
        std::cerr << "Failed to parse file: " << filePath << " with error code: " << result << std::endl;
        return;
    }

    // 处理PDF元数据信息
    Document document = engine->getDocument();
    // Print and verify document metrics and metadata
    print_document_metadata(document.metadata);
    print_document_metrics(document.metrics);

    // 处理PDF每一页的元数据信息
    int pageSize = 0;
    engine->getPageSize(&pageSize);
    for (int i = 0; i < pageSize; ++i) {
        const auto &current_page = document.pages[i];
        print_page_info(current_page, i);
    }

    // 处理PDF每一页的表格数据信息
    for (int i = 0; i < pageSize; ++i) {
        int tableSize = 0;
        engine->getPageTablesSize(i, &tableSize);
        std::cout << "Page [" << i << "] Table Size: " << tableSize << std::endl;

        if (tableSize == 0) continue;

        std::vector<Table> tables(tableSize);
        result = engine->findTables(i, tables.data(), tableSize);
        if (result != ERROR_CODE_TYPE::ERR_OK) {
            std::cerr << "Failed to find tables for page: " << i << " with error code: " << result << std::endl;
            continue;
        }

        for (size_t j = 0; j < std::min(tables.size(), static_cast<size_t>(kMaxPrintTable)); j++) {
            print_table_info(tables[j], j);
            print_table_cells(tables[j]);
            std::cout << std::endl;
        }
    }

    engine->release();
}

// main函数为额外逻辑，处理了文件读取和日志文件收集，不展示API使用方法
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << std::endl;
    }

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
