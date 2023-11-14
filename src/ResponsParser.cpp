#include "../include/Multiplexer.hpp"

void Multiplexer::sendResponseToClient(Client clientData)
{
  (void)clientData;
  std::cout << clientData.fields["host:"] << std::endl;
}
