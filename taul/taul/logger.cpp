

#include "logger.h"

#include <iostream>


void taul::stdout_logger::on_log(const std::string& msg) {
    std::cout << msg << "\n";
}

void taul::stderr_logger::on_log(const std::string& msg) {
    std::cerr << msg << "\n";
}

std::shared_ptr<taul::logger> taul::make_stdout_logger() {
    return internal::std_logger_lazy_loader::load_stdout();
}

std::shared_ptr<taul::logger> taul::make_stderr_logger() {
    return internal::std_logger_lazy_loader::load_stderr();
}

std::shared_ptr<taul::logger> taul::stdout_lgr() {
    return make_stdout_logger();
}

std::shared_ptr<taul::logger> taul::stderr_lgr() {
    return make_stderr_logger();
}

std::shared_ptr<taul::logger> taul::internal::std_logger_lazy_loader::load_stdout() {
    if (!_stdout) {
        std::scoped_lock lock(_stdout_mtx);
        _stdout = std::make_shared<stdout_logger>();
    }
    return _stdout;
}

std::shared_ptr<taul::logger> taul::internal::std_logger_lazy_loader::load_stderr() {
    if (!_stderr) {
        std::scoped_lock lock(_stderr_mtx);
        _stderr = std::make_shared<stderr_logger>();
    }
    return _stderr;
}

std::shared_ptr<taul::logger> taul::internal::std_logger_lazy_loader::_stdout = nullptr;
std::shared_ptr<taul::logger> taul::internal::std_logger_lazy_loader::_stderr = nullptr;
std::mutex taul::internal::std_logger_lazy_loader::_stdout_mtx = std::mutex{};
std::mutex taul::internal::std_logger_lazy_loader::_stderr_mtx = std::mutex{};

