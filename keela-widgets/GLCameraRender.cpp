//
// Created by brand on 6/13/25.
//

#include "keela-widgets/GLCameraRender.h"

#include <gio/gio.h>
#include <glad/glad.h>
#include <glibmm/main.h>
#include <spdlog/spdlog.h>

#include <utility>

#include "keela-pipeline/consts.h"

Keela::GLCameraRender::GLCameraRender(std::shared_ptr<PresentationBin> bin) {
	GError *error = nullptr;
	spdlog::debug("{}: Loading vertex shader resource", __func__);
	auto vertex_res =
	    g_resources_lookup_data("/org/gatech/keela/shaders/video-vertex.glsl", G_RESOURCE_LOOKUP_FLAGS_NONE, &error);
	if(!vertex_res && !error) {
		std::stringstream ss;
		ss << "Could not load vertex shader resource: " << error->message;
		throw std::runtime_error(ss.str());
	}
	spdlog::debug("{}: Loading fragment shader resource", __func__);
	auto fragment_res =
	    g_resources_lookup_data("/org/gatech/keela/shaders/video-fragment.glsl", G_RESOURCE_LOOKUP_FLAGS_NONE, &error);
	if(!fragment_res && !error) {
		std::stringstream ss;
		ss << "Could not load fragment shader resource: " << error->message;
		throw std::runtime_error(ss.str());
	}

	auto vertex_shader_size = g_bytes_get_size(vertex_res);
	auto vertex_shader_data = g_bytes_get_data(vertex_res, &vertex_shader_size);
	auto vertex_shader_dup = g_strndup(static_cast<const gchar *>(vertex_shader_data), vertex_shader_size);

	vertexShaderSource = std::string(vertex_shader_dup);
	// uncommenting these appear to crash the program. it's somewhat unclear who owns these
	g_bytes_unref(vertex_res);
	g_free(vertex_shader_dup);

	auto frag_shader_size = g_bytes_get_size(fragment_res);
	auto frag_shader_data = g_bytes_get_data(fragment_res, &frag_shader_size);
	auto frag_shader_dup = g_strndup(static_cast<const gchar *>(frag_shader_data), frag_shader_size);

	fragmentShaderSource = std::string(frag_shader_dup);
	g_bytes_unref(fragment_res);
	g_free(frag_shader_dup);
	this->bin = std::move(bin);
	Glib::signal_timeout().connect(sigc::mem_fun(this, &GLCameraRender::on_timeout), 16);
}

Keela::GLCameraRender::~GLCameraRender() {
	spdlog::debug(__func__);
}

void Keela::GLCameraRender::new_tex_sample(GstSample *sample) {
	assert(sample != nullptr);
	auto buf = gst_sample_get_buffer(sample);
	assert(buf != nullptr);

	auto caps = gst_sample_get_caps(sample);
	assert(caps != nullptr);

	auto structure = gst_caps_get_structure(caps, 0);
	assert(structure != nullptr);

	gint width, height;
	bool ret = false;
	ret = gst_structure_get_int(structure, "width", &width);
	ret &= gst_structure_get_int(structure, "height", &height);
	auto fmt = std::string(gst_structure_get_string(structure, "format"));
	assert(ret);
	spdlog::trace("New tex sample width: {} height: {} format: {}", width, height, fmt);
	GstMapInfo mapInfo;
	if(gst_buffer_map(buf, &mapInfo, GST_MAP_READ)) {
		auto tex_fmt = fmt == GRAY8 ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT;

		// if pixel format is greater than 8 bits, check if we need to swap bytes
		if(fmt == GRAY16_BE && std::endian::native != std::endian::big) {
			glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_TRUE);
		} else if(fmt == GRAY16_LE && std::endian::native != std::endian::little) {
			glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_TRUE);
		}
		// otherwise, we can just use host order
		else {
			glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
		}

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, tex_fmt, mapInfo.data);

		gst_buffer_unmap(buf, &mapInfo);
	} else {
		std::stringstream ss;
		ss << __func__ << ": Could not map sample buffer";
		throw std::runtime_error(ss.str());
	}
	gst_sample_unref(sample);
}

void Keela::GLCameraRender::on_realize() {
	spdlog::info("GLCameraRender::{}", __func__);
	GLArea::on_realize();
	make_current();

	// make sure opengl symbols are loaded before we do any opengl stuff
	if(!gladLoadGL()) {
		throw std::runtime_error("Failed to load OpenGL symbols");
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char *vertex_cstr = vertexShaderSource.c_str();
	glShaderSource(vertexShader, 1, &vertex_cstr, nullptr);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::stringstream ss;
		ss << "Vertex shader compilation failed: " << infoLog;
		spdlog::error("error in vertex shader\n{}", vertexShaderSource);
		throw std::runtime_error(ss.str());
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char *fragment_cstr = fragmentShaderSource.c_str();
	glShaderSource(fragmentShader, 1, &fragment_cstr, nullptr);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::stringstream ss;
		ss << "Fragment shader compilation failed: " << infoLog;
		spdlog::error("error in fragment shader\n{}", fragmentShaderSource);
		throw std::runtime_error(ss.str());
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::stringstream ss;
		ss << "Program linking failed: " << infoLog;
		throw std::runtime_error(ss.str());
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set nearest neighbors interpolation
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

bool Keela::GLCameraRender::on_render(const Glib::RefPtr<Gdk::GLContext> &context) {
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);

	// check for a new sample
	GstSample *sample = nullptr;

	g_signal_emit_by_name(bin->sink, "try-pull-sample", 0, &sample, nullptr);
	if(sample) {
		new_tex_sample(sample);
	}

	glUseProgram(shaderProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shaderProgram, "videoTexture"), 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	return GLArea::on_render(context);
}

bool Keela::GLCameraRender::on_timeout() {
	queue_render();
	return true;
}
