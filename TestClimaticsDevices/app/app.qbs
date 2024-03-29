import qbs

CppApplication
{
	consoleApplication: true
	targetName: "TestClimaticsDevices"
	Depends {name: "Qt"; submodules: ["core","gui","network"]}
	Depends {name: "libwliotproxy-static"}

	Group
	{
		name: "wliotproxy app"
		fileTagsFilter: "application"
		qbs.install: true
		qbs.installDir: "usr/sbin"
	}

	files:[
        "DeviceStructs.h",
        "ModelCallback.cpp",
        "ModelCallback.h",
        "Outdoor.cpp",
        "Outdoor.h",
        "Room.cpp",
        "Room.h",
        "RoomCallback.cpp",
        "RoomCallback.h",
        "SimpleRoom.cpp",
        "SimpleRoom.h",
        "SimpleRoomCallback.cpp",
        "SimpleRoomCallback.h",
        "StdQFile.cpp",
        "StdQFile.h",
        "climatics_defines.h",
        "main.cpp",
    ]
}
