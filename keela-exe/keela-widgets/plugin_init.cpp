#include "plugin_init.h"

#include <gst/gst.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace Keela {
std::filesystem::path get_executable_path() {
#ifdef _WIN32
	char path[MAX_PATH];
	DWORD length = GetModuleFileNameA(NULL, path, MAX_PATH);
	if(length > 0) {
		return std::filesystem::path(path);
	}
#else
	return std::filesystem::canonical("/proc/self/exe");
#endif
	return std::filesystem::path();  // fallback
}

void initialize_plugins() {
#ifdef KEELA_VIDEOTESTSRC_ENABLED
	// Determine plugin path relative to executable location
	std::filesystem::path exe_path = get_executable_path();
	if(exe_path.empty()) {
		spdlog::warn("Could not determine executable path - custom plugins not available");
		return;
	}

	std::filesystem::path build_dir = exe_path.parent_path().parent_path();
	std::filesystem::path plugin_path = build_dir / "plugins";

	if(std::filesystem::exists(plugin_path)) {
		std::string plugin_path_str = plugin_path.string();

		if(g_setenv("GST_PLUGIN_PATH", plugin_path_str.c_str(), TRUE)) {
			spdlog::debug("Set GST_PLUGIN_PATH to: {}", plugin_path_str);
			gst_registry_scan_path(gst_registry_get(), plugin_path_str.c_str());
			spdlog::debug("Scanned plugin path for custom plugins");
		} else {
			spdlog::error("Failed to set GST_PLUGIN_PATH");
		}
	} else {
		spdlog::warn("Plugin directory not found: {} - custom plugins not available", plugin_path.string());
	}
#else
	spdlog::debug("Custom plugins disabled at build time");
#endif
}
}  // namespace Keela
