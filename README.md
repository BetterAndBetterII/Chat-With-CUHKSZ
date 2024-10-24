# Chat-With-CUHKSZ
 
## 开发指南

### 选择正确的CMakeLists.txt

- 前端 `CMakeLists.txt` 位于 `frontend` 目录下
- 后端 `CMakeLists.txt` 位于 `backend` 目录下

### 当你需要测试某个模块时

比如我今天写好了BlackBoardSystem， 我希望测试一下它的功能是否正常，那么我需要在 `backend` 目录下的 `test` 目录新建一个 `test_blackboard` 文件，然后在 `CMakeLists.txt` 文件中修改如下：

```cmake
add_executable(backend_executable ./test/test_blackboard.cpp   // 这里的test_blackboard.cpp是你新建的测试文件
        ${BACKEND_SOURCES})
```

也就是将你的测试文件加入到 `add_executable` 中，然后重新编译即可。

### 当你需要添加新的模块时

你需要将你的模块放在 `backend` 目录下的 `src` 目录中，头文件放在 `include` 目录中，然后在 `CMakeLists.txt` 文件不需要修改，因为它会自动扫描 `src` 目录下的所有文件。

**注意：** 如果你的模块需要依赖其他模块，请在 `CMakeLists.txt` 文件中添加依赖。

### 当你需要添加新的依赖时

你需要在 `CMakeLists.txt` 文件中添加依赖，比如我今天需要添加一个新的依赖 `libcurl`，那么我需要在 `CMakeLists.txt` 文件中添加如下代码：

```cmake
find_package(CURL REQUIRED)
target_link_libraries(backend_executable CURL::libcurl)
```

## 开发计划

1. 第一阶段
- [ ] 前端完成一个简单的界面，打印Helloworld
- [ ] 服务器完成一个简单的接口/helloworld，返回HTTPResponse text/plain Helloworld
- [ ] 后端完成BlackBoardSystem
- [ ] Model实现一个简单的大模型调用
- [ ] Agent实现简单自循环，实现随机动作（查询，执行，总结输出），实现历史记录存储
- [ ] Knowledge构思完成，实现一个简单的文本存储

