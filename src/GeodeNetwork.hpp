#pragma once

#include <functional>
#include <Geode/utils/web.hpp>

class GeodeNetwork {
public:
    enum HttpMethod {
        MGet
    };
protected:
    std::function<void(GeodeNetwork *)> _onOk = nullptr;
    std::function<void(GeodeNetwork *)> _onError = nullptr;

    std::string _data;
    std::string _url;

    HttpMethod _method = MGet;

    geode::EventListener<geode::utils::web::WebTask> _listener;

    void setupListener();
public:
    GeodeNetwork();

    void setOkCallback(std::function<void(GeodeNetwork *)> ok);
    void setErrorCallback(std::function<void(GeodeNetwork *)> error);

    void setURL(std::string url);
    void setMethod(HttpMethod method);

    std::string &getResponse();

    void send();
};