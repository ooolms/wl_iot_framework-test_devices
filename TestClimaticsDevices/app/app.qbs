import qbs

QtApplication
{
	cpp.cxxLanguageVersion: "c++11"
	consoleApplication: true
	targetName: "TestClimaticsDevices"
	Depends {name: "libwliotproxy"}

	Group
	{
		name: "wliotproxy app"
		fileTagsFilter: "application"
		qbs.install: true
		qbs.installDir: "usr/sbin"
	}

	files:[
        "StdQFile.cpp",
        "StdQFile.h",
        "main.cpp",
    ]
}
