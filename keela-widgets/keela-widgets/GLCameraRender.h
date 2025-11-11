//
// Created by brand on 6/13/25.
//

#ifndef GLCAMERARENDER_H
#define GLCAMERARENDER_H
#include <gstreamer-1.0/gst/gst.h>
#include <gtkmm/glarea.h>
#include <keela-pipeline/presentationbin.h>

namespace Keela {
class GLCameraRender final : public Gtk::GLArea {
   public:
    GLCameraRender(std::shared_ptr<PresentationBin> bin);

    ~GLCameraRender() override;

    void new_tex_sample(GstSample *sample);

   protected:
    void on_realize() override;

    bool on_render(const Glib::RefPtr<Gdk::GLContext> &context) override;

   private:
    std::string vertexShaderSource;
    std::string fragmentShaderSource;

    bool on_timeout();

    unsigned int VBO;
    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;
    unsigned int VAO;
    unsigned int texture;
    std::shared_ptr<PresentationBin> bin;
    float vertices[24] = {-1, -1, 0, 1, 1, 1, 1, 0, -1, 1, 0, 0, 1, -1, 1, 1, 1, 1, 1, 0, -1, -1, 0, 1};
};
}  // namespace Keela
#endif  // GLCAMERARENDER_H
