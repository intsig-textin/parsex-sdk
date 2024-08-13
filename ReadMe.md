## 整体介绍
x2md_tools 提供了一套sdk， 帮助开发者解析[pdf_to_markdown](https://www.textin.com/document/x_to_markdown)的结果，获取对应的版面元素的数据结构


## 版本相关
| 版本         | 说明                                     |
|------------|----------------------------------------|
| 1.0.0（当前版本）      | 支持提取json中`table`数据与`cell`数据，支持Linux C版本与Python版本      |


## 快速开始

### 基本的使用方式

#### Linux SDK
处理表格数据：
```cpp
ITextInParserEngine* engine = ITextInParserEngine::createAndStartTextInParserEngine();
engine->parseFile(filePath.c_str()); // filePath为需要解析的json文件路径
int pageSize = 0;
engine->getPageSize(&pageSize)
for (int page_i = 0; page_i < pageSize; ++page_i) {
  int tableSize = 0;
  engine->getPageTablesSize(page_i, &tableSize);
  std::vector<Table> tables(tableSize);
  engine->findTables(page_i, tables.data(), tableSize);
  for (const auto& table : tables) {
    print_table_info(table); // 可以看到每一个表格的数据信息
  }
}
engine->release();
delete engine;
```

#### Python SDK
处理表格数据：
```python
engine = textin_parser.ITextInParserEngine.createAndStartTextInParserEngine()
engine.parseFile("file.json")
result, page_size = engine.getPageSize()
if page_size > 0:
    # 获取第一页的表格数量
    result, table_size = engine.getPageTablesSize(0)
    # 获取第一页的表格数组
    result, tables = engine.findTables(0, table_size)
        for i, table in enumerate(tables):
            print_table(table)
```

# Linux SDK介绍

## 代码框架
- `linux_sdk/include`目录包含所需调用所用到的头文件
- `linux_sdk/samples`目录包含获取表格信息的演示用程序代码与pdf/json文件
- `linux_sdk/src`目录包含功能实现的具体源文件
- `linux_sdk/third_party`目录包含编译所需要的第三方代码文件

## 编译运行
- 环境准备: gcc/g++ 8.1及以上
- sample编译：到`samples/cpp`目录下，直接运行`./build.sh`即可编译出测试程序
- sample运行会遍历`resource/`的json文件，sample会处理这些json文件，结果都输出在`output/log.txt`中
- 每轮执行都会创建新的`output/log.txt`

## 运行样例
- 项目clone下来之后，可以进入`samples/cpp`目录，运行`./test_program`
- 在`output/log.txt`中即可看到`samples/resource`内的演示表格输出

## 使用方式


### SDK API使用说明

- `createAndStartTextInParserEngine`获取一个Parser的实列
- `parse` 解析json的原始字符串
- `parseFile` 根据json文件路径读取json数据解析
- `release` 释放Parser内部的资源
- `getPageSize` 获取PDF中的Page数量
- `getPageTablesSize` 获取具体Page的表格数量
- `findTables` 获取具体Page的表格
- `getDocument` 获取整个PDF数据的结构体

## Sample代码框架
### 说明
- `samples/base`目录包含对应的工具函数文件
- `samples/cpp`目录包含演示的main函数以及可直接运行的`Makefile`与`build.sh`编译脚本
- `samples/file`目录包含演示用的pdf文件
- `samples/resource`目录包含file文件中通过TEXTIN RESTAPI返回的json结果并保存成了json文件（为了方便演示，直接展示处理`samples/resource`目录里json）
- output目录包含输出的结果文件log.txt


# Python SDK介绍
Python API是对Cpp中接口通过Pybind11做的二次封装。所以和Linux SDK提供的C++ API功能完全一样，每次需要运行`build_so.sh`脚本构建指定`python3`版本的so

## 编译运行
- 环境准备: gcc/g++ 8/9 python3为3.6及其以上
- 进入`python_sdk/`，运行`build_so.sh`即可在当前目录下获得与`python3`版本一致的so，注：如果出现`pybind11/pybind11.h: No such file or directory`请再执行一次`build_so.sh`
- 生成得到so后使用`python3`运行`python_sdk/`下的`parser_test.py`，即可在`output.log`看到所展示的对当前目录下`chinese-tables.json`的解析结果
- 开发者可以使用`import textin_parser`来访问so里提供的接口




