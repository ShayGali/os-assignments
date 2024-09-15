#pragma once

#include "command_handler.hpp"

class PipelineHandler : public CommandHandler {
   public:
    string handle(string input, int user_fd) override;
};
