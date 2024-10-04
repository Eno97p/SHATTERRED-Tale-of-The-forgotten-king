



::EffectTool------------------------------------------------------------------------------------
::Engine
xcopy			/y			.\Engine\Bin\Engine.dll				.\EffectTool\Bin\

::Blast
xcopy			/y/s		.\Engine\DLL\Blast\Release\*.*		.\EffectTool\Bin\

::ASSIMP
xcopy			/y/s		.\Engine\DLL\Assimp\Debug\*.*		.\EffectTool\Bin\
xcopy			/y/s		.\Engine\DLL\Assimp\Release\*.*		.\EffectTool\Bin\

::FOMOD
xcopy			/y/s		.\Engine\DLL\FMOD\*.*				.\EffectTool\Bin\

::PhysX
xcopy			/y/s		.\Engine\DLL\PhysX\Debug\*.*		.\EffectTool\Bin\


::ShaderFiles
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsl		.\EffectTool\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsli	.\EffectTool\Bin\ShaderFiles\
::EffectTool------------------------------------------------------------------------------------