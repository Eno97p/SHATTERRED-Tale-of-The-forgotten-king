




::MYMapTool------------------------------------------------------------------------------------
::Engine_DLL
xcopy			/y			.\Engine\Bin\Engine.dll				.\MYMapTool\Bin\

::Blast_DLL
xcopy			/y/s		.\Engine\DLL\Blast\Release\*.*		.\MYMapTool\Bin\

::ASSIMP_DLL
xcopy			/y/s		.\Engine\DLL\Assimp\Debug\*.*		.\MYMapTool\Bin\
xcopy			/y/s		.\Engine\DLL\Assimp\Release\*.*		.\MYMapTool\Bin\

::FOMOD_DLL
xcopy			/y/s		.\Engine\DLL\FMOD\*.*				.\MYMapTool\Bin\

::PhysX_DLL
xcopy			/y/s		.\Engine\DLL\PhysX\Debug\*.*		.\MYMapTool\Bin\


::ShaderFiles
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsl		.\MYMapTool\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsli	.\MYMapTool\Bin\ShaderFiles\
::MYMapTool------------------------------------------------------------------------------------