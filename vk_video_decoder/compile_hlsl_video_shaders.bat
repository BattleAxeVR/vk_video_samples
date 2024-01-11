REM glslangValidator.exe video_vertex_shader.vert.hlsl -o video_vertex_shader.spv --target-env vulkan1.3
REM glslangValidator.exe video_pixel_shader.frag.hlsl -o video_pixel_shader.spv --target-env vulkan1.3

REM glslcc.exe --vert=video_vertex_shader.vert --output=video_vertex_shader.vert.hlsl --lang=hlsl
REM glslcc.exe --frag=video_pixel_shader.frag --output=video_pixel_shader.frag.hlsl --lang=hlsl

REM dxc -Zi -E main -Od -T vs_6_6 -nologo -spirv -fspv-target-env=vulkan1.3 video_vertex_shader.vert.hlsl /E"main" /Fo"video_vertex_shader.spv" /nologo 
REM dxc -Zi -E main -Od -T ps_6_6 -nologo -spirv -fspv-target-env=vulkan1.3 video_pixel_shader.frag.hlsl /E"main" /Fo"video_pixel_shader.spv" /nologo 

REM slangc video_vertex_shader.vert.hlsl -entry main -stage vertex -o video_vertex_shader.spv
slangc video_pixel_shader.frag.hlsl -entry main -stage fragment -o video_pixel_shader.spv