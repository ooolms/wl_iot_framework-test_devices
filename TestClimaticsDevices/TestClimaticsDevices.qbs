import qbs

Project
{
	references: [
		"app",
		"../../wl_iot_framework/wliotproxy-src-base/libwliotproxy-base",
		"../../wl_iot_framework/wliotproxy-src-base/libVDIL",
		"../../wl_iot_framework/wliotproxy-src-client/libwliotproxy-static"
	]
}
