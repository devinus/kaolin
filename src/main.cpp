/*
 * Kaolin - Exfoliate your face
 * Copyright (c) 2008 Devin Torres
 * Licensed under the MIT license.
 */

#include <sys/socket.h>
#include <signal.h>
#include <cstdio>
#include <iostream>
#include <string>
#include "config.h"
#include "main.h"
#include "server.h"
#include "util.h"


/*
 * Global server object.
 */
server *s = new server;



/*
 * Main function declaration.
 */
int main(int argc, char* argv[])
{
  if (argc > 1)
  {
    if (!util::is_number(argv[1]))
    {
      std::cout << "Usage: " << argv[0] << " [port #]\n";
      return 0;
    }

    port = atoi(argv[1]);

    if (port != 23 && port <= 1024)
    {
      std::cout << "Port number must be 23 or above 1024.\n";
      return 0;
    }
  }

  gettimeofday(&s->current_time, NULL);

  s->log("Creating host...\n");

  /*
   * So the program doesn't crash if player reset by peer.
   */
  signal(SIGPIPE, SIG_IGN);

  sockaddr_in addr;

  try
  {
    if ((s->host = socket(AF_INET, SOCK_STREAM, 0)) < 0)
      throw "socket";

    const int opt = 1;

    if (setsockopt(s->host, SOL_SOCKET, SO_REUSEADDR,
      reinterpret_cast<const char*>(&opt), sizeof(opt)) < 0)
        throw "setsockopt";

    memset(&addr, 0, sizeof(addr));

    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port);

    if (bind(s->host, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
        throw "bind";

    if (listen(s->host, 10) < 0)
      throw "listen";
  }
  catch (const std::string& str)
  {
    s->error("%s: %d: %s: %s\n", __FILE__, __LINE__, __func__, str.c_str());
    exit(EXIT_FAILURE);
    return 0;
  }

  s->log("Listening on port %d.\n", port);

  s->load_world();
  s->game_loop(addr);

  delete s;

  return 0;
}
