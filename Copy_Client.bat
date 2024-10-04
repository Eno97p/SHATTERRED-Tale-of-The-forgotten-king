
::Client------------------------------------------------------------------------------------
::Engine
xcopy			/y		.\Engine\Bin\Engine.dll					.\Client\Bin\
xcopy			/y		.\Engine\Bin\Engine.dll					.\Client\Bin\Release\

::Blast
xcopy			/y/s		.\Engine\DLL\Blast\Debug\*.*		.\Client\Bin\
xcopy			/y/s		.\Engine\DLL\Blast\Release\*.*		.\Client\Bin\Release\

::ASSIMP
xcopy			/y/s		.\Engine\DLL\Assimp\Debug\*.*		.\Client\Bin\
xcopy			/y/s		.\Engine\DLL\Assimp\Release\*.*		.\Client\Bin\Release\

::FOMOD
xcopy			/y/s		.\Engine\DLL\FMOD\*.*				.\Client\Bin\
xcopy			/y/s		.\Engine\DLL\FMOD\*.*				.\Client\Bin\Release\

::PhysX
xcopy			/y/s		.\Engine\DLL\PhysX\Debug\*.*		.\Client\Bin\
xcopy			/y/s		.\Engine\DLL\PhysX\Release\*.*		.\Client\Bin\Release\

::ShaderFiles
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsl			.\Client\Bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.hlsli		.\Client\Bin\ShaderFiles\
::Client------------------------------------------------------------------------------------






