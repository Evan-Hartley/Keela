#include <gtkmm/application.h>

#include "mainwindow.h"
//
// Created by brand on 5/25/2025.
//
#include <keela-widgets/shader-resources.h>
#include <plugin_init.h>

#include <iostream>

int main(int argc, char **argv) {
	spdlog::set_level(spdlog::level::debug);
	spdlog::info("Registering shader resources");
	g_resources_register(shader_resources_get_resource());
	spdlog::info("Creating app");
	auto app = Gtk::Application::create(argc, argv, "com.gatech.keela");
	gst_init(&argc, &argv);

	spdlog::info("Initializing GStreamer plugins, if available");
	Keela::initialize_plugins();

	MainWindow window;
	spdlog::info("Starting application");
	return app->run(window);
}
