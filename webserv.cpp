//
// Created by Sungjun Park on 2022/12/15.
//
#include "Server.hpp"

WS::Server* G_SERVER;

int main()
{
  WS::Server server;
  G_SERVER = &server;
  server.run();
}