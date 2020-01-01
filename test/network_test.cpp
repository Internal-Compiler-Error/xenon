#define CATCH_CONFIG_MAIN
#include "../src/network/network.hpp"
#include <catch2/catch.hpp>

auto url = skyr::url{"https://download.fedoraproject.org/pub/fedora/linux/releases/31/Workstation/x86_64/iso/"
                     "Fedora-Workstation-Live-x86_64-31-1.9.iso"};

TEST_CASE("subdownload name is correct")
{
    auto [success, response]                = xenon::network::can_be_accelerated(url);
    auto                     final_location = xenon::network::get_final_location(url);
    xenon::network::download download{*response, final_location}(0, <#initializer #>, skyr::url())(<#initializer #>, <#initializer #>, skyr::url());
    download.complete_all_downloads();
}


TEST_CASE("Acceleration detection works")
{
    REQUIRE(xenon::network::can_be_accelerated(url).first);
}

TEST_CASE("Get final location works")
{
    REQUIRE_NOTHROW(xenon::network::get_final_location(url));
}

TEST_CASE("fuck")
{
    auto [success, response]       = xenon::network::can_be_accelerated(url);
    auto            final_location = xenon::network::get_final_location(url);
    httplib::Client cli(final_location.host());
    std::ofstream   out{"out", std::ios::trunc & std::ios::binary};
    cli.set_follow_location(true);
    auto res = cli.Get(final_location.pathname().c_str(), [&](char const* data, uint64_t length) {
        out.write(data, length);
        return true;
    });
}

TEST_CASE("sub download works")
{
    auto [success, response]                    = xenon::network::can_be_accelerated(url);
    auto                         final_location = xenon::network::get_final_location(url);
    xenon::network::sub_download sub_download{
        final_location,
        0,
        "file name",
        {0, 1'929'379'840},
        (std::ios::out & std::ios::binary &
         std::ios::trunc)}(<#initializer #>, skyr::url(), 0, <#initializer #>, <#initializer #>, _S_ate);
}
