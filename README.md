# TokenEngine
Token of allegiance for vulkan

## 第三方开源库支持
vendor 文件夹下内容
++++ GLFW               窗口支持
++++ glm                数学计算库
++++ imgui              UI支持
++++ spdlog             日志支撑
++++ stb                图片导入处理
++++ tinyobjloader      3D模型导入


## 编译流程
环境：
1、Windows 系统
2、安装Vulkan SDK
3、VS2022 或  VS2019

步骤：
1、双击 Run_VS2022.bat；
2、打开 TokenEngine.sln;

若安装的是VS2019 修改 Run_VS2022.bat文件，
替换<call Tools\premake5.exe vs2022> 为 <call Tools\premake5.exe vs2019>
