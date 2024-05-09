

project "Tests"
	
	kind "ConsoleApp"
	targetname "tests"
	
	files
	{
		"**.h",
		"**.cpp",
		"**.c",
		"**.txt",
		"**.taul",
        "../vendor/googletest/googletest/src/gtest-all.cc"
	}
	
	includedirs
	{
		"../taul",
		"../vendor/googletest/googletest/include",
		"../vendor/googletest/googletest/"
	}
	
	links
	{
		"TAUL"
	}

