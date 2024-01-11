REM WORKS
REM dxc -Zi -E main -Od -T vs_6_6 -nologo -spirv -fspv-target-env=vulkan1.3 video_vertex_shader.vert.hlsl /E"main" /Fo"video_vertex_shader.spv" /nologo 

REM DOESN'T WORK
REM dxc -Zi -E main -Od -T ps_6_6 -nologo -spirv -fspv-target-env=vulkan1.3 video_pixel_shader.frag.hlsl /E"main" /Fo"video_pixel_shader.spv" /nologo 

REM WORKS
REM slangc video_vertex_shader.vert.hlsl -entry main -stage vertex -o video_vertex_shader.spv

REM DOESN'T WORK
slangc video_pixel_shader.frag.hlsl -entry main -stage fragment -o video_pixel_shader.spv