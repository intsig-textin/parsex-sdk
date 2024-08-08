#ifndef _ITEXT_IN_LINUXSDKCOMMON_H_
#define _ITEXT_IN_LINUXSDKCOMMON_H_

#include <cstdint>
#include <string>
#include <string.h>

namespace intsig {
namespace textin {
namespace linuxsdk {

class IEngine{
    virtual ~IEngine() {};
};

class IEngineConfig {
    virtual ~IEngineConfig() {};
};

typedef unsigned char uchar_t;
typedef unsigned int uint_t;
typedef unsigned int uid_t;
typedef uint64_t u64_t;

/** Error codes.
 *
 */
enum ERROR_CODE_TYPE {

    /** 0: No error. */
    ERR_OK = 0,
    ERR_FAILED = 1,
    ERR_INVALID_ARGUMENT = 2,
    ERR_INTERNAL_FAILED = 3,
    ERR_EMPTY_PAGE = 4,
    ERR_BEFORE_PARSE = 5,
    ERR_NULL_BUFFER = 6,
    ERR_ID_OUT_RANGE = 7,
    ERR_EMPTY_TABLE = 8,
    ERR_FILE_NOT_FOUND = 9,

};

enum DOCUMENT_PARSER_CODE {
    PAGE_NULL = 0
};

/** State codes. */
enum STAT_CODE_TYPE {

};

}
}
}

#endif
