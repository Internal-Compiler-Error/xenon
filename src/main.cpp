#include <gui/window.hpp>

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create("com.billwang2001.xenon");
    xenon::gui::window instance;
    return app->run(instance, argc, argv);
}
