#include "window.hpp"

#include "network.hpp"
#include <gtkmm.h>
#include <gtkmm/aboutdialog.h>
#include <gtkmm/liststore.h>
#include <gtkmm/textview.h>
#include <skyr/url.hpp>

xenon::gui::window::window()
{
        // set up attributes concerning the whole window
        set_default_size(500, 500);
        set_title("Xenon");

        // the header_bar_ serves both as the title and a menubar
        set_titlebar(header_bar_);

        // add_ the top level container to the window
        add(top_box_);

        // this is the main area of display
        top_box_.pack_start(scrolled_window_);

        // add_ the download column
        scrolled_window_.add(download_columns_view_);
        scrolled_window_.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

        // the list_store object actually stores data, the model only describes data structure
        list_store_ = Gtk::ListStore::create(column_model_);
        download_columns_view_.set_model(list_store_);    // the view actually presents data

        // set up the titles of each column
        download_columns_view_.append_column("index", column_model_.download_index_);
        download_columns_view_.append_column("name", column_model_.download_name_);
        // we want to display a progress bar instead of just a number
        auto* cell = Gtk::make_managed<Gtk::CellRendererProgress>();
        download_columns_view_.append_column("Progress", *cell);
        auto* progress_column = download_columns_view_.get_column(2);
        progress_column->add_attribute(cell->property_value(), column_model_.download_progress_);

        // we want all columns to be resizable
        for (int i = 0; i < 3; ++i)
        {
                auto* column = download_columns_view_.get_column(i);
                column->set_resizable(true);
        }


        download_scheduler_dispatcher_.connect([this] { handle(); });
        // when we are all done_, show all the stuff
        show_all_children();

        download_scheduler_loop_thread_ = std::thread{[this] { download_scheduler_.main_loop(); }};
}

void
    xenon::gui::window::notify(enum event event)
{
        most_recent_event_ = event;
        download_scheduler_dispatcher_.emit();
}

void
    xenon::gui::window::update_list_store()
{
        for (auto const& item: download_scheduler_.download_queue_)
        {
                if (download_to_row_map_.count(&item) == 0)
                {
                        auto row                              = *(list_store_->append());
                        download_to_row_map_[&item]           = row;
                        row[column_model_.download_index_]    = std::to_string(item.index_);
                        row[column_model_.download_progress_] = item.total_progress_;
                        row[column_model_.download_name_]     = item.file_name_;
                }
                else
                {
                        auto& row_iter                        = download_to_row_map_[&item];
                        auto& row                             = *row_iter;
                        row[column_model_.download_progress_] = item.total_progress_;
                }
        }
}

xenon::gui::window::~window()
{
        download_scheduler_.stop();
        if (download_scheduler_loop_thread_.joinable())
        {
                download_scheduler_loop_thread_.join();
        }
}

void
    xenon::gui::window::on_new_download()
{

        auto const& new_download              = download_scheduler_.download_queue_.back();
        auto        row_iter                  = list_store_->append();
        auto&       row                       = *row_iter;
        row[column_model_.download_index_]    = std::to_string(new_download.index_);
        row[column_model_.download_progress_] = new_download.total_progress_;
        row[column_model_.download_name_]     = new_download.file_name_;

        download_to_row_map_[&new_download] = row_iter;
}

void
    xenon::gui::window::handle()
{
        std::lock_guard<std::mutex> guard{mtx_};
        switch (most_recent_event_)
        {
                case event::download_completed:
                        // todo: i don't have to do anything for this case??
                        break;
                // todo: fix this later
                case event::new_download_created:
                case event::progress_update: update_list_store(); break;
        }
}

xenon::gui::download_column_model::download_column_model()
{
        add(download_index_);
        add(download_name_);
        add(download_progress_);
}


xenon::gui::download_column_model&
    xenon::gui::download_column_model::get_instance()
{
        static download_column_model instance;
        return instance;
}

xenon::gui::header_bar::header_bar(window* window) : window_{window}
{
        // general set up
        set_title("Xenon");
        set_show_close_button();
        pack_start(new_);
        pack_start(about_);

        // set up what the button does when clicked
        about_.signal_clicked().connect([] {
                about_dialog about_dialog;
                about_dialog.run();
        });

        new_.signal_clicked().connect([this] {
                new_download_dialog dialog{window_};
                dialog.run();
        });

        show_all_children();
}

xenon::gui::about_dialog::about_dialog() : Gtk::AboutDialog{true}
{
        set_title("Xenon");
        set_name("Xenon");
        set_logo_default();
        set_authors({"Bill Wang"});
        set_copyright("Bill Wang 2019");
        set_comments("Xenon is a high performance download tool built using modern C++");
        set_license_type(Gtk::License::LICENSE_GPL_2_0);
}


xenon::gui::new_download_dialog::new_download_dialog(xenon::gui::window* window) : header_bar_{this}, window_{window}
{

        set_titlebar(header_bar_);
        auto* vbox = get_content_area();
        vbox->pack_start(user_helper_text_, true, false);
        vbox->pack_start(user_url_input_, true, false);

        header_bar_.add_.signal_clicked().connect([this] {
                // todo: this is were the hard work should be done
                // am i done???
                auto user_url_text = user_url_input_.get_text();
                try
                {
                        skyr::url url{static_cast<std::string>(user_url_text)};
                        auto [success, response] = network::can_be_accelerated(url);
                        if (success)
                        {
                                auto final_location = network::get_final_location(url);
                                window_->download_scheduler_.add_download(*response, final_location);
                                close();
                        }
                        else
                        {
                                download_not_supported_dialog dialog{this};
                                dialog.run();
                        }
                }
                catch (std::exception& e)
                {
                        url_parse_error_dialog dialog{this};
                        dialog.run();
                }
        });
        show_all_children();
}

xenon::gui::new_download_dialog::cancel_or_add_header_bar::cancel_or_add_header_bar(Gtk::Window* parent) : parent_{parent}
{
        set_title("New Download");
        pack_start(cancel_);
        pack_end(add_);

        cancel_.signal_clicked().connect([this] { parent_->close(); });
}

xenon::gui::download_not_supported_dialog::download_not_supported_dialog(Gtk::Window* parent) :
    Gtk::MessageDialog{*parent,
                       "this download is not supported due to the lack of a Content-Type header",
                       false,
                       Gtk::MessageType::MESSAGE_ERROR,
                       Gtk::ButtonsType::BUTTONS_OK,
                       true}
{
}

xenon::gui::url_parse_error_dialog::url_parse_error_dialog(Gtk::Window* parent) :
    Gtk::MessageDialog{*parent, "error while parsing url", false, Gtk::MessageType::MESSAGE_ERROR, Gtk::ButtonsType::BUTTONS_OK, true}
{
}
