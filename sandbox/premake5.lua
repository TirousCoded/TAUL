

project "Sandbox"
	
	kind "ConsoleApp"
	targetname "sandbox"
	
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

