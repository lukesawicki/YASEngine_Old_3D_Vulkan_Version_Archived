C:\VulkanSDK\1.1.77.0\Bin32\glslangValidator.exe -V Shaders\vertShader.vert
C:\VulkanSDK\1.1.77.0\Bin32\glslangValidator.exe -V Shaders\fragShader.frag
REM cd Shaders

REM files are created in folder where is this script
REM copy /Y Shaders\vert.spv ..\
REM copy /Y Shaders\frag.spv ..\
copy /Y vert.spv Shaders\
copy /Y frag.spv Shaders\