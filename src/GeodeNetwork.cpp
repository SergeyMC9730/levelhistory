#include "GeodeNetwork.hpp"

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

    geode::utils::web::WebTask task;
    
    if (_method == MGet) {
        task = req.get(_url);
    }

    _listener.setFilter(task);
}

GeodeNetwork::GeodeNetwork() {
    setupListener();
}

void GeodeNetwork::setupListener() {
    _listener.bind([this] (geode::utils::web::WebTask::Event* e) {
        if (geode::utils::web::WebResponse* res = e->getValue()) {
            this->_data = res->string().unwrapOr("Not a string");

            if (res->ok() && this->_onOk != nullptr) {
                this->_onOk(this); 

                return;
            }

            if (!res->ok() && this->_onError != nullptr) {
                this->_onError(this); 

                return;
            }
        } else if (e->isCancelled()) {
            this->_data = "Error: cancelled";

            if (this->_onError != nullptr) {
                this->_onError(this); 

                return;
            }
        }
    });
}