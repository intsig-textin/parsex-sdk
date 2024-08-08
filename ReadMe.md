## 快速开始
- [产品介绍](https://www.textin.com/market/detail/pdf_to_markdown)
- [C++ API]()

## 编译运行
- 环境准备: gcc/g++ 8.1及以上
- sample编译：到samples/cpp目录下，直接运行build.sh即可编译出测试程序
- sample运行: 运行./test_program 即可看到相关用法，加上相关输出
- sample运行会遍历resource的json结果，sample会处理这些json结果得到输出在output/log.txt中
- sample中也演示了如何从file里获取这些json文件并同步对这些json文件处理
## 运行样例
- resource目录里存放演示用的json预处理信息
```

## Sample代码框架
### 说明
- ITextInParserEngine 接口类包含应用程序调用的主要方法和暴露的成员结构
### 代码框架
- include/ITextInParserEngine.h 所有程序调用API
- include/ITextInLinuxSdkCommon.h  textin sdk所有程序调用回用到的错误返回值
