#include <gtkmm/application.h>

#include "mainwindow.h"
//
// Created by brand on 5/25/2025.
//
#include <keela-widgets/shader-resources.h>
#include <plugin_init.h>
#include <spdlog/cfg/env.h>

#include <iostream>

int main(int argc, char **argv) {
	spdlog::cfg::load_env_levels();
	spdlog::info("Registering shader resources");
	g_resources_register(shader_resources_get_resource());
	spdlog::info("Creating app");
	auto app = Gtk::Application::create(argc, argv, "com.gatech.keela");
	gst_init(&argc, &argv);

	spdlog::info("Initializing GStreamer plugins, if available");
	Keela::initialize_plugins();

	// Get the icon:
	Gtk::IconTheme keela_theme = Gtk::IconTheme::get_default();
	try {
		Gdk::Pixbuf icon = keela_theme.load_icon("keela-icon", 256, 0);
		button::image = new Gtk::Image::from_pixbuf(icon);
		Gtk::IconTheme keela_theme = Gtk::IconTheme::get_default();
		Gtk::IconTheme::gtk_window_set_default_icon_name("keela-icon")
	} catch(Error e) {
		warning(e.message);
	}

	MainWindow window;
	spdlog::info("Starting application");
	return app->run(window);
}
