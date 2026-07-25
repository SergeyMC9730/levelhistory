#include "GeodeNetwork.hpp"
using namespace geode::prelude;

void GeodeNetwork::setOkCallback(std::function<void(GeodeNetwork *)> ok) {
    _onOk = ok;
}
void GeodeNetwork::setErrorCallback(std::function<void(GeodeNetwork *)> error) {
    _onError = error;
}

void GeodeNetwork::setURL(std::string url) {
    _url = url;
}
void GeodeNetwork::setMethod(HttpMethod method) {
    _method = method;
}

std::string &GeodeNetwork::getResponse() {
    return _data;
}

void GeodeNetwork::send() {
    geode::utils::web::WebRequest req = geode::utils::web::WebRequest();

    req.timeout(std::chrono::seconds(10));

    if (_method == MGet) {
        _listener.spawn(req.get(_url), [this](geode::utils::web::WebResponse res) {
            this->_data = res.string().unwrapOr("Not a string");

            if (res.ok() && this->_onOk != nullptr) {
                this->_onOk(this);

                return;
            }

            if (!res.ok() && this->_onError != nullptr) {
                this->_onError(this);

                return;
            }
        });
    }
}

GeodeNetwork::GeodeNetwork() {
}
