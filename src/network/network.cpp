#include "network.hpp"
#include <httplib.h>

#include <utility>
#include <window.hpp>

using namespace httplib;

std::pair<bool, std::shared_ptr<httplib::Response>> xenon::network::can_be_accelerated(skyr::url const &url)
{
        Client client{url.host()};

        client.set_follow_location(true);
        auto respond = client.Head(url.pathname().c_str(), {{"User-Agent", user_agent_info}});

        return std::make_pair(respond->has_header("Content-Length"), std::move(respond));
}

skyr::url xenon::network::get_final_location(skyr::url const &request)
{
        auto constexpr found = 302;
        auto constexpr redirect = 307;
        Client client{request.host()};

        auto respond = client.Head(request.pathname().c_str(), {{"User-Agent", "Xenon/0.1"}});

        if (respond->status == found or respond->status == redirect)
        {
                skyr::url redirected_location{respond->get_header_value("Location")};
                return get_final_location(redirected_location);
        }
        return request;
}

xenon::network::sub_download::sub_download(download &parent, skyr::url const &request_url, int index,
                                           std::string const &http_file_name, std::pair<int64_t, int64_t> const &range,
                                           std::ios_base::openmode openmode)
    : parent_download_{parent}, index_{index}, request_url_{&request_url},
      file_name_{std::string{http_file_name}.append(std::to_string(index))}, file_{file_name_, openmode}
{
        auto const &[begin, end] = range;

        total_size_ = end - begin + 1;

        http_headers_.emplace("User-Agent", user_agent_info);
        http_headers_.emplace(httplib::make_range_header({range}));

        assert(file_.is_open());
        thread = thread_t{[this] { start(); }};
}
void xenon::network::sub_download::start()
{
        httplib::Client client{request_url_->host()};
        client.set_follow_location(true);
        auto content_retriever = [this](const char *data, uint64_t data_length) -> bool {
                return on_write(data, data_length);
        };

        client.Get(request_url_->pathname().c_str(), http_headers_, content_retriever);

        file_.flush();
        file_.close();
        done_ = true;
}
bool xenon::network::sub_download::on_write(const char *data, uint64_t length)
{
        file_.write(data, length);
        downloaded_size_ += length;
        progress_ = static_cast<double>(downloaded_size_) / static_cast<double>(total_size_) * 100;

        parent_download_.report_progress(this, progress_);
        return true;
}
xenon::network::sub_download::~sub_download()
{
        if (thread.joinable())
        {
                thread.join();
        }
}
void xenon::network::sub_download::join()
{
        if (thread.joinable())
        {
                join();
        }
}
xenon::network::download::download(int index, httplib::Response const &response, skyr::url request_url)
    : index_{index}, request_url_{std::move(request_url)}, file_name_{request_url_.pathname().substr(
                                                               request_url_.pathname().find_last_of('/') + 1)},
      total_size_{std::stoll(response.get_header_value("Content-Length"))}
{
        auto const remainder = total_size_ % sub_count_;
        auto const each_size = (total_size_ - remainder) / sub_count_;

        sub_downloads_.reserve(sub_count_);

        auto constexpr open_modes = std::ios_base::binary | std::ios_base::trunc;
        for (auto i = 0; i != (sub_count_ - 1); ++i)
        {
                auto pair = std::make_pair(each_size * i, each_size * (i + 1) - 1);
                auto &ref = sub_downloads_.emplace_back(*this, request_url_, i, file_name_, pair, open_modes);
                progress_map_[&ref] = 0;
        }
        {
                auto const begin = each_size * (sub_count_ - 1);
                auto const end = each_size * sub_count_ + remainder - 1;
                auto const range = std::make_pair(begin, end);

                auto &ref =
                    sub_downloads_.emplace_back(*this, request_url_, sub_count_ - 1, file_name_, range, open_modes);
                progress_map_[&ref] = 0;
        }
}
xenon::network::download::~download()
{
        complete_all_downloads();
}
void xenon::network::download::complete_all_downloads()
{
        for (auto &item : sub_downloads_)
        {
                item.thread.join();
        }

        if (!done_)
        {
                std::ofstream out{file_name_, std::ios_base::binary | std::ios_base::trunc};
                for (auto const &downloads : sub_downloads_)
                {
                        std::ifstream file{downloads.file_name_, std::ios_base::binary};
                        out << file.rdbuf();
                }
                done_ = true;
        }
}

double xenon::network::download::progress()
{
        return total_progress_;
}
void xenon::network::download::report_progress(xenon::network::sub_download *self, int progress)
{
        std::lock_guard<std::mutex> guard{*mtx_};
        progress_map_[self] = progress;

        int total_progress{0};
        for (auto const &pair : progress_map_)
        {
                total_progress += pair.second;
        }
        total_progress_ = static_cast<double>(total_progress) / static_cast<double>((100 * sub_count_)) * 100;
}
bool xenon::network::download::is_done() const
{
        return done_;
}

void xenon::network::download_scheduler::main_loop()
{
        while (!stop_signal_received)
        {
                download_queue_.erase(std::remove_if(download_queue_.begin(), download_queue_.end(),
                                                     [](download const &download) { return download.is_done(); }),
                                      download_queue_.end());

                window_->notify(gui::event::progress_update);
                std::this_thread::sleep_for(0.5s);
        }
}
xenon::network::download_scheduler::download_scheduler(xenon::gui::window *window) : window_{window}
{
}
void xenon::network::download_scheduler::add_download(httplib::Response const &response, skyr::url request_url)
{
        download_queue_.emplace_back(download_index_++, response, std::move(request_url));
        window_->notify(gui::event::new_download_created);
}
void xenon::network::download_scheduler::stop()
{
        stop_signal_received = true;
}
