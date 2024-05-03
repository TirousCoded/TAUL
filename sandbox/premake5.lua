

project "Sandbox"
	
	kind "ConsoleApp"
	
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

