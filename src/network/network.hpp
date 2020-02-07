#ifndef XENON_NETWORK_HPP
#define XENON_NETWORK_HPP

#include <atomic>
#include <bits/stdc++.h> // todo: figure out what headers I actually require
#include <cstdlib>
#include <fstream>
#include <gtkmm.h>
#include <misc/hack_jthread.hpp>
#include <httplib.h>
#include <iostream>
#include <memory>
#include <regex>
#include <skyr/url.hpp>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

namespace xenon::gui {
class window;
}

namespace xenon::network {

auto constexpr user_agent_info = "xenon/0.1";

/**
 * Establish a connection to the url and see if Content-Length header is present
 * @param url the request destination
 * @return the first bool indicates the presence of the Content-Length header,
 * the second shared_ptr is the response given back from the connection,
 * regardless of the success
 */
std::pair<bool, std::shared_ptr<httplib::Response>> can_be_accelerated(skyr::url const &url);

skyr::url get_final_location(skyr::url const &request);

/**
 * A download would contain a collection of sub_downloads. Each sub_download
 * holds a Body and some necessary information
 */

struct download;

struct sub_download {

    using thread_t = misc::hack_jthread;

    download &parent_download_;
    int index_; // index of the sub_download in the perspective of the download
    skyr::url const *request_url_;
    httplib::Headers http_headers_; // all headers in the request
    std::string file_name_;         // the name of the download
    std::ofstream file_;            // actual Body
    int64_t total_size_;            // size of the sub_download
    int64_t downloaded_size_{0};    // the size downloaded
    int progress_{0};               // a progress_map_ from 0 - 1
    bool done_{false};
    thread_t thread; // the thread to carry out the hard work, NOTE: THIS *MUST* BE DECLARED LAST BECAUSE WE WANT
                     // IT'S DESTRUCTOR TO BE RUN FIRST!

    sub_download(sub_download const &) = delete;

    sub_download(sub_download &&) = default;

    // todo: see this actually needs to be movable
    // sub_download& operator=(sub_download&&) = default;

    sub_download(download &parent, skyr::url const &request_url, int index, std::string const &http_file_name,
                 std::pair<int64_t, int64_t> const &range, std::ios_base::openmode openmode);

    void join();

    ~sub_download();

private:
    void start();

    bool on_write(const char *data, uint64_t length);
};

/**
 * When a url is successfully parsed and a valid connection is established, a
 * download object would be created. It consists of a collection of sub
 * downloads
 */

struct download {
    download(int index, httplib::Response const &response, skyr::url request_url);

    int index_;
    skyr::url request_url_; // the request url
    std::string file_name_; // the name of file from url
    int64_t total_size_;    // the total size given by the http connection
    int sub_count_ = static_cast<int>(std::thread::hardware_concurrency()); // the number of sub_counts is
    // the number of threads
    std::vector<sub_download> sub_downloads_; // the sub downlaods
    std::map<sub_download *, int> progress_map_;
    bool done_{false};
    std::unique_ptr<std::mutex> mtx_ = std::make_unique<std::mutex>();
    int total_progress_{0};

    // todo: consider making this private and friending sub_download
    void report_progress(sub_download *self,
                         int progress); // this is called by sub_downloads to report their progress
    bool is_done() const;

    double progress();

    download(download &&) noexcept = default;

    download &operator=(download &&) noexcept = default;

    ~download();

private:
    void complete_all_downloads();
};

void do_download();

class download_scheduler {
    friend gui::window;

private:
    std::vector<download> download_queue_;
    gui::window *window_;
    int download_index_{0};
    bool stop_signal_received{false};
    std::mutex mtx_;

public:
    explicit download_scheduler(gui::window *window);

    void add_download(httplib::Response const &response, skyr::url request_url);

    void stop();

    void main_loop();
};

} // namespace xenon::network

#endif // XENON_NETWORK_HPP
