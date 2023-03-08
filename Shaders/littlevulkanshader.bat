set VULKAN_PATH=%VULKAN_SDK%


%VULKAN_PATH%/Bin/glslc.exe simple_shader.vert -o simple_shader.vert.spv
%VULKAN_PATH%/Bin/glslc.exe simple_shader.frag -o simple_shader.frag.spv
%VULKAN_PATH%/Bin/glslc.exe point_light.vert -o point_light.vert.spv
%VULKAN_PATH%/Bin/glslc.exe point_light.frag -o point_light.frag.spv

pause