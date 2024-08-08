## 快速开始
- [产品介绍](https://www.textin.com/market/detail/pdf_to_markdown)
- [SDK介绍]()

## 代码框架
- include目录包含所需调用所用到的头文件
- samples目录包含获取表格信息的演示用程序代码与pdf/json文件
- src目录包含功能实现的具体源文件
- third_party目录包含编译所需要的第三方代码文件

## 编译运行
- 环境准备: gcc/g++ 8.1及以上
- sample编译：到samples/cpp目录下，直接运行build.sh即可编译出测试程序
- sample运行会遍历resource的json结果，sample会处理这些json结果得到输出在output/log.txt中
- sample中也演示了如何从file里获取这些json文件并同步对这些json文件处理

## 运行样例
- 项目clone下来之后，可以进入samples/cpp目录，运行./test_program即可看到samples/resource内的演示表格输出

## 使用方式

### 基本的使用方式

处理表格数据：
```cpp
ITextInParserEngine* engine = ITextInParserEngine::createAndStartTextInParserEngine();
engine->parseFile(filePath.c_str()); // filePath为需要解析的json文件路径
int pageSize = 0;
engine->getPageSize(&pageSize)
for (int page_i = 0; i < pageSize; ++page_i) {
  int tableSize = 0;
  engine->getPageTablesSize(i, &tableSize);
  std::vector<Table> tables(tableSize);
  engine->findTables(page_i, tables.data(), tableSize);
  for (const auto& table : tables) {
    print_table_info(table); // 可以看到每一个表格的数据信息
  }
}
engine->release();
delete engine;
```

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
- base目录包含对应的工具函数文件
- cpp目录包含演示的main函数以及可直接运行的Makefile与build.sh编译脚本
- file目录包含演示用的pdf文件
- resource目录包含file文件中通过rest api返回的json结果（为了方便演示，直接展示处理resource目录里json）
- output目录包含输出的结果文件log.txt
