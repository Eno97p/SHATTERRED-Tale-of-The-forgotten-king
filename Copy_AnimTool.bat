

::AnimTool------------------------------------------------------------------------------------
::Engine
xcopy			/y			.\Engine\Bin\Engine.dll				.\AnimTool\Bin\

::Blast
xcopy			/y/s		.\Engine\DLL\Blast\Release\*.*		.\AnimTool\Bin\

::ASSIMP
xcopy			/y/s		.\Engine\DLL\Assimp\Debug\*.*		.\AnimTool\Bin\
xcopy			/y/s		.\Engine\DLL\Assimp\Release\*.*		.\AnimTool\Bin\

::FOMOD
xcopy			/y/s		.\Engine\DLL\FMOD\*.*				.\AnimTool\Bin\

::PhysX
xcopy			/y/s		.\Engine\DLL\PhysX\Debug\*.*		.\AnimTool\Bin\


::ShaderFiles
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsl		.\AnimTool\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsli	.\AnimTool\Bin\ShaderFiles\
::AnimTool------------------------------------------------------------------------------------