

project "Examples"
	
	kind "ConsoleApp"
	targetname "examples"
	
	files
	{
		"**.h",
		"**.cpp",
		"**.c",
		"**.txt",
		"**.taul"
	}
	
	includedirs
	{
		"../taul"
	}
	
	links
	{
		"TAUL"
	}

