// Copyright (c) 2024 Intsig.net. All rights reserved

// This program is confidential and proprietary to Intsig.net.
// And may not be copied, reproduced, modified, disclosed to others, published
// or used, in whole or in part, without the express prior written permission
// of Intsig.net.

#ifndef _ITEXT_IN_PARSERENGINE_H_
#define _ITEXT_IN_PARSERENGINE_H_
#include "ITextInLinuxSdkCommon.h"

#ifndef OPTIONAL_ENUM_SIZE_T
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
#define OPTIONAL_ENUM_SIZE_T enum : size_t
#else
#define OPTIONAL_ENUM_SIZE_T enum
#endif
#endif

#if !defined(__APPLE__)
#define __deprecated
#endif

namespace intsig {
namespace textin {
namespace parser_engine {

/**
 * @struct Pos
 * @brief Represents the position of a rectangle.
 */
typedef struct Pos {
    /**
     * @brief Top-left x coordinate
     */
    int x1;
    /**
     * @brief Top-left y coordinate
     */
    int y1;
    /**
     * @brief Top-right x coordinate
     */
    int x2;
    /**
     * @brief Top-right y coordinate
     */
    int y2;
    /**
     * @brief Bottom-right x coordinate
     */
    int x3;
    /**
     * @brief Bottom-right y coordinate
     */
    int y3;
    /**
     * @brief Bottom-left x coordinate
     */
    int x4;
    /**
     * @brief Bottom-left y coordinate
     */
    int y4;

    /**
     * @brief Constructor to initialize position coordinates to zero.
     */
    Pos() : x1(0), y1(0), x2(0), y2(0), x3(0), y3(0), x4(0), y4(0) {}

    /**
     * @brief Constructor to initialize position coordinates with given values.
     * @param x1_ Top-left x coordinate
     * @param y1_ Top-left y coordinate
     * @param x2_ Top-right x coordinate
     * @param y2_ Top-right y coordinate
     * @param x3_ Bottom-right x coordinate
     * @param y3_ Bottom-right y coordinate
     * @param x4_ Bottom-left x coordinate
     * @param y4_ Bottom-left y coordinate
     */
    Pos(int x1_, int y1_, int x2_, int y2_, int x3_, int y3_, int x4_, int y4_)
        : x1(x1_), y1(y1_), x2(x2_), y2(y2_), x3(x3_), y3(y3_), x4(x4_), y4(y4_) {}
} Pos;

/**
 * @struct ImageContent
 * @brief Represents the content of an image.
 */
typedef struct ImageContent {
    OPTIONAL_ENUM_SIZE_T {
        /** The maximum value (px) of the width. */
        kImageSizeArr = 2,
    };
    /**
     * @brief Type of the stamp
     */
    const char* stamp_type;
    /**
     * @brief Color of the stamp
     */
    const char* stamp_color;
    /**
     * @brief Sub-type of the image
     */
    const char* sub_type;

    /**
     * @brief Pointer to the size array
     */
    int* size;
    /**
     * @brief Size of the size array
     */
    int size_size;

    /**
     * @brief Position of the image
     */
    Pos pos;

    /**
     * @brief Base64 encoded image data
     */
    const char* base64;
    /**
     * @brief Path to the image file
     */
    const char* path;

    ImageContent() : stamp_type(NULL), stamp_color(NULL), sub_type(NULL), size(NULL), size_size(0), base64(NULL), path(NULL) {}
} ImageContent;

/**
 * @struct Imageblock
 * @brief Represents a block of images.
 */
typedef struct Imageblock {
    OPTIONAL_ENUM_SIZE_T {
        /** The maximum value (px) of the width. */
        kMaxImageContentCount = 1000,
    };
    /**
     * @brief Position of the image block
     */
    Pos pos;
    /**
     * @brief Z-order of the image block
     */
    int zorder;
    /**
     * @brief Sub-type of the image block
     */
    const char* sub_type;

    /**
     * @brief Pointer to the image content array
     */
    ImageContent* i_contents;
    /**
     * @brief Size of the image content array
     */
    int content_size;


    Imageblock() : zorder(0), sub_type(NULL), i_contents(NULL), content_size(0) {}
} Imageblock;

/**
 * @struct TextContent
 * @brief Represents the content of text.
 */
typedef struct TextContent {
    OPTIONAL_ENUM_SIZE_T {
        /** The maximum value (px) of the width. */
        kMaxTextBlockCharCount = 10000,
    };

    /**
     * @struct Cand
     * @brief Represents a candidate string.
     */
    struct Cand {
        /**
         * @brief Candidate string
         */
        const char* cand_str;

        Cand() : cand_str(NULL) {}

        Cand(const char * cand_str_) : cand_str(cand_str_) {}
    };

    /**
     * @struct CandScore
     * @brief Represents a candidate score.
     */
    struct CandScore {
        /**
         * @brief Candidate score
         */
        float score;

        CandScore() : score(0.0f) {}

        CandScore(float score_) : score(score_) {}
    };

    /**
     * @brief Text content
     */
    const char* text;
    /**
     * @brief Direction of the text
     */
    int direction;
    /**
     * @brief Score of the text
     */
    int score;
    /**
     * @brief Angle of the text
     */
    int angle;

    /**
     * @brief Pointer to the character positions
     */
    Pos* char_pos = NULL;
    /**
     * @brief Size of the character positions array
     */
    int char_pos_size;

    /**
     * @brief Pointer to the character candidates
     */
    Cand* char_cand = NULL;
    /**
     * @brief Size of the character candidates array
     */
    int char_cand_size;

    /**
     * @brief Pointer to the character candidate scores
     */
    CandScore* char_cand_score = NULL;
    /**
     * @brief Size of the character candidate scores array
     */
    int char_cand_score_size;

    TextContent() : text(NULL), direction(0), score(0), angle(0), char_pos(NULL), char_pos_size(0), char_cand(NULL),
        char_cand_size(0), char_cand_score(NULL), char_cand_score_size(0) {}
} TextContent;

/**
 * @struct Textblock
 * @brief Represents a block of text.
 */
typedef struct Textblock {
    OPTIONAL_ENUM_SIZE_T {
        /** The maximum value (px) of the width. */
        kMaxTextContentCount = 1000,
    };
    /**
     * @brief Position of the text block
     */
    Pos pos;
    /**
     * @brief Sub-type of the text block
     */
    const char* sub_type;
    /**
     * @brief Indicates if the text block continues
     */
    bool is_continue;

    /**
     * @brief Pointer to the text content array
     */
    TextContent* t_contents;
    /**
     * @brief Size of the text content array
     */
    int content_size;

    Textblock() : sub_type(NULL), is_continue(false), t_contents(NULL), content_size(0) {}
} Textblock;

/**
 * @struct Cell
 * @brief Represents a cell in a table.
 */
typedef struct Cell {
    OPTIONAL_ENUM_SIZE_T {
        /** The maximum value (px) of the width. */
        kMaxTextBlockCount = 50,
        /** The maximum value (px) of the width. */
        kMaxImageBlockCount = 20,
    };
    /**
     * @brief Position of the cell
     */
    Pos pos;
    /**
     * @brief Row index of the cell
     */
    int row;
    /**
     * @brief Column index of the cell
     */
    int col;

    /**
     * @brief Pointer to the text blocks array
     */
    Textblock* t_blocks;
    /**
     * @brief Size of the text blocks array
     */
    int t_block_size;

    /**
     * @brief Pointer to the image blocks array
     */
    Imageblock* i_blocks;
    /**
     * @brief Size of the image blocks array
     */
    int i_block_size;

    Cell() : row(0), col(0), t_blocks(NULL), t_block_size(0), i_blocks(NULL), i_block_size(0) {}
} Cell;

/**
 * @struct Table
 * @brief Represents a table.
 */
typedef struct Table {
    OPTIONAL_ENUM_SIZE_T {
        /** The maximum value (px) of the width. */
        kMaxColWidthCount = 100,
        /** The maximum value (px) of the height. */
        kMaxRowHeightCount = 100,
        /** The maximum value (fps) of the cell count. */
        kMaxCellsCount = 500,
    };
    /**
     * @brief Sub-type of the table
     */
    const char* sub_type;
    /**
     * @brief Position of the table
     */
    Pos pos;
    /**
     * @brief Number of rows in the table
     */
    int rows;
    /**
     * @brief Number of columns in the table
     */
    int cols;

    /**
     * @brief Pointer to the columns width array
     */
    int* colums_width;
    /**
     * @brief Size of the columns width array
     */
    int colums_width_size;

    /**
     * @brief Pointer to the rows height array
     */
    int* rows_height;
    /**
     * @brief Size of the rows height array
     */
    int rows_height_size;

    /**
     * @brief Indicates if the table continues
     */
    bool is_continue;

    /**
     * @brief Pointer to the cells array
     */
    Cell* cells;
    /**
     * @brief Size of the cells array
     */
    int cell_size;

    Table() : sub_type(NULL), rows(0), cols(0), colums_width(NULL), colums_width_size(0), rows_height(NULL), rows_height_size(0), is_continue(0), cells(NULL), cell_size(0) {}
} Table;

/**
 * @struct Page
 * @brief Represents a page.
 */
typedef struct Page {
    OPTIONAL_ENUM_SIZE_T {
        /** The maximum value (px) of the width. */
        kMaxPageTableCount = 10,
        /** The minimum value (px) of the width. */
        kMinPageTableCount = 0
    };
    /**
     * @brief Page ID
     */
    int page_id;
    /**
     * @brief Status of the page
     */
    const char* page_status;
    /**
     * @brief Image ID of the page
     */
    const char* page_image_id;
    /**
     * @brief Duration of the page
     */
    float page_durations;

    /**
     * @brief Pointer to the tables array
     */
    Table* page_tables;
    /**
     * @brief Size of the tables array
     */
    int table_size;

    Page() : page_id(-1), page_status(NULL), page_image_id(NULL), page_durations(0.0f), page_tables(NULL), table_size(0) {}
} Page;

/**
 * @struct MetaData
 * @brief Represents metadata information.
 */
typedef struct MetaData {
    /**
     * @brief Version of the document
     */
    const char* version;
    /**
     * @brief Markdown content
     */
    const char* markdown;
    /**
     * @brief Duration of the document
     */
    float duration;

    MetaData() : version(NULL), markdown(NULL), duration(0.0f) {}
} MetaData;

/**
 * @struct Metrics
 * @brief Represents metrics information.
 */
typedef struct Metrics {
    /**
     * @brief Total number of pages
     */
    int page_total;
    /**
     * @brief Number of valid pages
     */
    int page_valid;
    /**
     * @brief Number of paragraphs
     */
    int paragraph_num;
    /**
     * @brief Number of characters
     */
    int char_num;
    /**
     * @brief Type of the document
     */
    const char* document_type;

    Metrics() : page_total(0), page_valid(0), paragraph_num(0), char_num(0), document_type(NULL) {}
} Metrics;

/**
 * @struct Document
 * @brief Represents a document.
 */
typedef struct Document {
    OPTIONAL_ENUM_SIZE_T {
        /** The maximum value (px) of the width. */
        kMaxDocumentPages = 100,
        /** The minimum value (px) of the width. */
        kMinDocumentPages = 0,
    };
    /**
     * @brief Pointer to the pages array
     */
    Page* pages;
    /**
     * @brief Size of the pages array
     */
    int page_size = 0;
    /**
     * @brief Metrics information
     */
    Metrics metrics;
    /**
     * @brief Metadata information
     */
    MetaData metadata;
    /**
     * @brief Encryption state
     */
    int encrypted_state;

    Document() : pages(NULL),  page_size(0), encrypted_state(0) {}
} Document;

// /**
//  * @brief Creates a Pos object.
//  * @return A Pos object.
//  */
// Pos create_pos();

// /**
//  * @brief Creates an ImageContent object.
//  * @return An ImageContent object.
//  */
// ImageContent create_image_content();

// /**
//  * @brief Creates an Imageblock object.
//  * @return An Imageblock object.
//  */
// Imageblock create_image_block();

// /**
//  * @brief Creates a TextContent object.
//  * @return A TextContent object.
//  */
// TextContent create_text_content();

// /**
//  * @brief Creates a Textblock object.
//  * @return A Textblock object.
//  */
// Textblock create_text_block();

// /**
//  * @brief Creates a Cell object.
//  * @return A Cell object.
//  */
// Cell create_cell();

// /**
//  * @brief Creates a Table object.
//  * @return A Table object.
//  */
// Table create_table();

// /**
//  * @brief Creates a Page object.
//  * @return A Page object.
//  */
// Page create_page();

// /**
//  * @brief Creates a MetaData object.
//  * @return A MetaData object.
//  */
// MetaData create_metadata();

// /**
//  * @brief Creates a Metrics object.
//  * @return A Metrics object.
//  */
// Metrics create_metrics();

// /**
//  * @brief Creates a Document object.
//  * @return A Document object.
//  */
// Document create_document();

/**
 * @class ITextInParserEngine
 * @brief Interface for the TextIn parser engine.
 */
class ITextInParserEngine {
public:
    /**
     * @brief Creates and starts the TextIn parser engine.
     *
     * This function creates a new instance of the TextIn parser engine and starts it.
     *
     * @return A pointer to the ITextInParserEngine object.
     */
    static ITextInParserEngine* createAndStartTextInParserEngine();

    /**
     * @brief Destructor.
     *
     * Releases the resources of the parser engine object.
     */
    virtual ~ITextInParserEngine() {}

    /**
     * @brief Parses raw data.
     *
     * This function takes a raw data string and parses it.
     *
     * @param raw A pointer to the string containing the raw data.
     * @return Returns a status code. 0 indicates success, non-zero indicates failure.
     */
    virtual int parse(const char* raw) = 0;

    virtual int parseFile(const char* path) = 0;
    /**
     * @brief Releases the parser engine object.
     *
     * This function releases the parser engine object and all its resources.
     *
     * @return Returns a status code. 0 indicates success, non-zero indicates failure.
     */
    virtual int release() = 0;

    /**
     * @brief Gets the number of pages.
     *
     * This function returns the number of pages in the document.
     *
     * @param page_size A pointer to an integer to store the number of pages.
     * @return Returns a status code. 0 indicates success, non-zero indicates failure.
     */
    virtual int getPageSize(int* page_size) = 0;

    /**
     * @brief Gets the number of tables in a page.
     *
     * This function returns the number of tables in the specified page.
     *
     * @param page_id The ID of the page.
     * @param table_size A pointer to an integer to store the number of tables.
     * @return Returns a status code. 0 indicates success, non-zero indicates failure.
     */
    virtual int getPageTablesSize(int page_id, int* table_size) = 0;

    /**
     * @brief Finds tables in a page.
     *
     * This function returns the table information in the specified page.
     *
     * @param page_id The ID of the page.
     * @param buffer A pointer to an array of Table structures to store the table information.
     * @param size The size of the buffer array.
     * @return Returns a status code. 0 indicates success, non-zero indicates failure.
     */
    virtual int findTables(int page_id, Table* buffer, int size) = 0;

    /**
     * @brief Gets document information.
     *
     * This function returns the information of the entire document.
     *
     * @return Returns a Document object containing detailed information about the document.
     */
    virtual Document getDocument() = 0;
};

}
}
}

#endif
