

#pragma once


#include <string>
#include <memory>
#include <format>
#include <mutex>


namespace taul {


    class logger {
    public:
        
        inline virtual ~logger() noexcept {}
        

        template<typename... Args>
        inline void log(std::format_string<Args...> fmt, Args&&... args);


    protected:

        virtual void on_log(const std::string& msg) = 0;
    };

    template<typename... Args>
    inline void taul::logger::log(std::format_string<Args...> fmt, Args&&... args) {
        on_log(std::format(fmt, std::forward<Args&&>(args)...));
    }


    template<typename... Args>
    inline void log(const std::shared_ptr<logger>& lgr, std::format_string<Args...> fmt, Args&&... args) {
        if (lgr) {
            lgr->log(fmt, std::forward<Args&&>(args)...);
        }
    }
}


// TAUL_LOG now uses short circuiting to avoid evaluation of taul::log
// in situations where lgr == nullptr, as in those situations it'll let
// us avoid things like potential heap usage in the arguments of taul::log

// thus, TAUL_LOG is preferred to taul::log for basic logging

#define TAUL_LOG(lgr, fmt, ...) \
((void)(bool(lgr) && (taul::log(lgr, fmt, __VA_ARGS__), true)))

// use a && short circuit to conditionally use TAUL_LOG

// the (~, true) wrapping TAUL_LOG is a ',' expression which
// will result in the 'true' being the final expression's value

#define TAUL_LOG_IF(cond, lgr, fmt, ...) \
((void)(bool(cond) && (TAUL_LOG(lgr, fmt, __VA_ARGS__), true)))


namespace taul {


    class stdout_logger final : public logger {
    public:

        stdout_logger() = default;
        stdout_logger(const stdout_logger&) = default;
        inline stdout_logger(stdout_logger&&) noexcept {};

        inline virtual ~stdout_logger() noexcept {}

        stdout_logger& operator=(const stdout_logger&) = default;
        inline stdout_logger& operator=(stdout_logger&&) noexcept {
            return *this;
        };


    protected:

        void on_log(const std::string& msg) override final;
    };

    class stderr_logger final : public logger {
    public:

        stderr_logger() = default;
        stderr_logger(const stderr_logger&) = default;
        inline stderr_logger(stderr_logger&&) noexcept {};

        inline virtual ~stderr_logger() noexcept {}

        stderr_logger& operator=(const stderr_logger&) = default;
        inline stderr_logger& operator=(stderr_logger&&) noexcept {
            return *this;
        };


    protected:

        void on_log(const std::string& msg) override final;
    };


    // these fns return loggers encapsulating logging to stdout/stderr

    // these each lazy load single global singleton logger, w/ each subsequent
    // call just returning these same loggers, w/ it thus being acceptable to
    // call these methods over-and-over w/out worry

    std::shared_ptr<logger> make_stdout_logger();
    std::shared_ptr<logger> make_stderr_logger();

    std::shared_ptr<logger> stdout_lgr();
    std::shared_ptr<logger> stderr_lgr();


    namespace internal {
        class std_logger_lazy_loader final {
        public:
            std_logger_lazy_loader() = default;


            static std::shared_ptr<logger> load_stdout();
            static std::shared_ptr<logger> load_stderr();


        private:
            static std::shared_ptr<logger> _stdout, _stderr;
            static std::mutex _stdout_mtx, _stderr_mtx;
        };
    }
}

