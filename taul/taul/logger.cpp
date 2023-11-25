

#include "logger.h"

#include <iostream>


void taul::stdout_logger::on_log(const std::string& msg) {
    std::cout << msg << "\n";
}

void taul::stderr_logger::on_log(const std::string& msg) {
    std::cerr << msg << "\n";
}

std::shared_ptr<taul::logger> taul::make_stdout_logger() {
    return std::make_shared<stdout_logger>();
}

std::shared_ptr<taul::logger> taul::make_stderr_logger() {
    return std::make_shared<stderr_logger>();
}
