

project "TAULC"
	
	kind "ConsoleApp"
	targetname "taulc"
	
	files
	{
		"**.h",
		"**.cpp",
		"**.c",
		"**.txt",
		"**.taul",
	}
	
	includedirs
	{
		"../taul",
	}
	
	links
	{
		"TAUL"
	}

