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
        "DeviceStructs.h",
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
