C:\VulkanSDK\1.1.77.0\Bin32\glslangValidator.exe -V Shaders\vertShader.vert
C:\VulkanSDK\1.1.77.0\Bin32\glslangValidator.exe -V Shaders\fragShader.frag
cd Shaders
copy /Y vert.spv ..\
copy /Y frag.spv ..\
cd..