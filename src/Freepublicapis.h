#ifndef FREEPUBLICAPIS_H
#define FREEPUBLICAPIS_H

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <pplx/pplx.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class Freepublicapis {
private:
    std::string api = "https://www.freepublicapis.com/api";
    http_client_config client_config;
    
    http_request createRequest(const std::string& endpoint, const std::string& method) {
        http_request request;
        
        if (method == "GET") {
            request.set_method(methods::GET);
        } else if (method == "POST") {
            request.set_method(methods::POST);
        } else if (method == "PUT") {
            request.set_method(methods::PUT);
        } else if (method == "DEL") {
            request.set_method(methods::DEL);
        }
        
        request.set_request_uri(utility::conversions::to_string_t(endpoint));
        
        // Set headers
        request.headers().add(U("Accept"), U("application/json"));
        request.headers().add(U("Content-Type"), U("application/json"));
        request.headers().add(U("User-Agent"), U("Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0"));
        
        return request;
    }
    
    std::string buildQueryParams(const std::map<std::string, std::string>& params) {
        if (params.empty()) return "";
        
        std::string query = "?";
        bool first = true;
        for (const auto& param : params) {
            if (!param.second.empty()) {
                if (!first) query += "&";
                auto encoded_value = web::uri::encode_data_string(utility::conversions::to_string_t(param.second));
                query += param.first + "=" + utility::conversions::to_utf8string(encoded_value);
                first = false;
            }
        }
        return query;
    }
    pplx::task<json::value> makeApiCall(const std::string& endpoint, const std::string& method) {
        http_client client(utility::conversions::to_string_t(api), client_config);
        auto request = createRequest(endpoint, method);

        return client.request(request)
            .then([](http_response response) {
                if (response.status_code() == status_codes::OK) {
                    return response.extract_json();
                } else {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("HTTP Error: ") + utility::conversions::to_string_t(std::to_string(response.status_code())));
                    error_obj[U("success")] = json::value::boolean(false);
                    return pplx::task_from_result(error_obj);
                }
            })
            .then([](pplx::task<json::value> previousTask) {
                try {
                    return previousTask.get();
                } catch (const std::exception& e) {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("Exception: ") + utility::conversions::to_string_t(e.what()));
                    error_obj[U("success")] = json::value::boolean(false);
                    return error_obj;
                }
            });
    }

public:
    Freepublicapis() {
        client_config.set_validate_certificates(false);
    }

    pplx::task<json::value> randomApi() {
        return makeApiCall("/random", "GET");
    }

    pplx::task<json::value> apiById(int apiId) {
        return makeApiCall("/apis/" + std::to_string(apiId), "GET");
    }

    pplx::task<json::value> getApiList(int limit=0,const std::string& sort="") {
        std::map<std::string, std::string> params;
        if (!sort.empty()) params["sort"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(sort)));
        if (limit > 0) params["limit"] = std::to_string(limit);
        return makeApiCall("/apis" + buildQueryParams(params), "GET");
    }

    pplx::task<json::value> analyzeDocs(const std::string& documentation) {
        json::value data;
        data[U("honeypot")] = json::value(utility::conversions::to_string_t(""));
        data[U("documentation")] = json::value(utility::conversions::to_string_t(documentation));
        return makeApiCall("/analyze-docs", "POST",data.serialize());
    }
};

#endif
