# TokenEngine
Token of allegiance for vulkan

## 第三方开源库支持
vendor 文件夹下内容<br>
++++ GLFW               窗口支持<br>
++++ glm                数学计算库<br>
++++ imgui              UI支持<br>
++++ spdlog             日志支撑<br>
++++ stb                图片导入处理<br>
++++ tinyobjloader      3D模型导入<br>


## 编译流程
环境：<br>
1、Windows 系统<br>
2、安装Vulkan SDK<br>
3、VS2022 或  VS2019<br>

步骤：<br>
1、双击 Run_VS2022.bat；<br>
2、打开 TokenEngine.sln;<br>

若安装的是VS2019 修改 Run_VS2022.bat文件，<br>
替换<call Tools\premake5.exe vs2022> 为 <call Tools\premake5.exe vs2019><br>


##  外部项目使用学习

//视频教程地址: https://www.youtube.com/watch?v=Y9U9IE0gVHA&list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR
[Little Vulkan Engine]  (https://github.com/blurrypiano/littleVulkanEngine)

//视频教程地址: https://www.youtube.com/watch?v=6isbso_GmUk&list=PLxxEkfi3SCmzo5i-7lYAkDuEZYSnOD5hq
[Vulkan-Tutorial-Advanced] (https://github.com/Mori-TM/Vulkan-Tutorial-Advanced)
