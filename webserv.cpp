//
// Created by Sungjun Park on 2022/12/15.
//
#include "Server.hpp"
#include <iostream>

int main()
{
  WS::Server server;
  auto& vServer = server.listen("127.0.0.1", "0.0.0.0", "80");
  vServer.setErrorPage("../contents/error.html");
  vServer.Get("/", [](HTTP::Request* req, HTTP::Response* res){
    res->sendFile("../contents/index.html");
  });
  vServer.Get("/img", [](HTTP::Request* req, HTTP::Response* res){
    auto URL = req->getPath();
    auto root = "../contents/img";
    auto path = root + URL.substr(URL.rfind('/'));
    res->sendFile(path.c_str());
  });
  vServer.Get("/favicon.ico", [](HTTP::Request* req, HTTP::Response* res){
      res->sendFile("../contents/favicon.ico");
  });
  server.run();
}