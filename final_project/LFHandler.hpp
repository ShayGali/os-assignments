#pragma once

#include "command_handler.hpp"

class LFHandler : public CommandHandler {
   public:
    string handle(string input, int user_fd) override;
};