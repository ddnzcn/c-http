#include "app/Routes.h"
#include "http_serv.h"
#include "router/Router.h"

int main() {
  // Initalize the routes
  init_routes();
  init_test_module();

  HTTP_SOCKET server = create_server(6969);
  // Starts the poll.
  start_listen_loop(server);
  return 0;
}
