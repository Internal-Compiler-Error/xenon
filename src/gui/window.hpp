#ifndef XENON_WINDOW_HPP
#define XENON_WINDOW_HPP

#include <atomic>
#include <gtkmm.h>
#include <mutex>
#include <network/network.hpp>
#include <thread>

namespace xenon::gui {
struct download_column_model : public Gtk::TreeModelColumnRecord {
    Gtk::TreeModelColumn<Glib::ustring> download_index_;
    Gtk::TreeModelColumn<Glib::ustring> download_name_;
    Gtk::TreeModelColumn<int> download_progress_;

    download_column_model();

    static download_column_model &get_instance();
};

/**
 * @brief The header bar for the main window
 */
class header_bar : public Gtk::HeaderBar {
private:
    Gtk::Button new_{Gtk::Stock::NEW};
    Gtk::Button about_{Gtk::Stock::ABOUT};
    window *window_;

public:
    explicit header_bar(window *window);
};

/**
 * @brief An about dialog to provide user for basic information about xenon
 */
struct about_dialog : public Gtk::AboutDialog {
    about_dialog();
};

/**
 * @brief The popup for users adding a new download to the queue
 */
struct new_download_dialog : public Gtk::Dialog {
    explicit new_download_dialog(window *window);

    /**
     * The header bar used exclusively by the new download dialog, it provides
     * an add and a cancel button for new download requests.
     */
    struct cancel_or_add_header_bar : Gtk::HeaderBar {
        explicit cancel_or_add_header_bar(Window *parent);

        Gtk::Window *parent_;
        Gtk::Button cancel_{Gtk::Stock::CANCEL};
        Gtk::Button add_{Gtk::Stock::ADD};
    };

    cancel_or_add_header_bar header_bar_{this};
    Gtk::Label user_helper_text_{"Enter valid URL here: "};
    Gtk::Entry user_url_input_;
    window *window_;

    ~new_download_dialog() override = default;
};

/**
 * @brief The error popup when the download cannot be supported
 */
struct download_not_supported_dialog : public Gtk::MessageDialog {
    explicit download_not_supported_dialog(Window *parent);
};

/**
 * @brief A popup on if the url cannot be parsed successfully
 */
struct url_parse_error_dialog : public Gtk::MessageDialog {
    explicit url_parse_error_dialog(Window *parent);
    ~url_parse_error_dialog() override = default;
};

enum class event { new_download_created, progress_update, download_completed };

/**
 * @brief The main xenon window
 */
class window : public Gtk::ApplicationWindow {
private:
    header_bar header_bar_{this};

    // top level container
    Gtk::VBox top_box_;

    // download column contained in a scrolled window
    Gtk::ScrolledWindow scrolled_window_;
    Glib::RefPtr<Gtk::ListStore> list_store_;
    Gtk::TreeView download_columns_view_;
    download_column_model &column_model_ = download_column_model::get_instance();
    Glib::Dispatcher download_scheduler_dispatcher_;
    std::thread download_scheduler_loop_thread_;
    std::map<network::download const *const, Gtk::ListStore::iterator> download_to_row_map_;
    std::mutex mtx_;
    std::atomic<gui::event> most_recent_event_;

public:
    network::download_scheduler download_scheduler_{this};
    void notify(enum event event); // this function is called by download_scheduler_ to update the list store
    void update_list_store();
    void on_new_download();
    void handle();

    window();
    ~window() override;
};

} // namespace xenon::gui

#endif // XENON_WINDOW_HPP
