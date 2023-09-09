#include <hex/helpers/logger.hpp>
#include <hex/helpers/fs.hpp>
#include <hex/helpers/fmt.hpp>

#include <wolv/io/file.hpp>

namespace hex::log::impl {

    static wolv::io::File s_loggerFile;
    std::mutex g_loggerMutex;

    std::vector<FILE *> getDestinations() {

        static std::vector<FILE *> destinations;

        if (destinations.empty()) {
            destinations.push_back(stdout);

            if (s_loggerFile.isValid())
                destinations.push_back(s_loggerFile.getHandle());
        }

        return destinations;
    }

    wolv::io::File& getFile() {
        return s_loggerFile;
    }

    bool isRedirected() {
        return s_loggerFile.isValid();
    }

    void redirectToFile() {
        if (s_loggerFile.isValid()) return;

        for (const auto &path : fs::getDefaultPaths(fs::ImHexPath::Logs, true)) {
            wolv::io::fs::createDirectories(path);
            s_loggerFile = wolv::io::File(path / hex::format("{0:%Y%m%d_%H%M%S}.log", fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))), wolv::io::File::Mode::Create);
            s_loggerFile.disableBuffering();

            if (s_loggerFile.isValid()) break;
        }
    }


    std::vector<LogEntry>& getLogEntries() {
        static std::vector<LogEntry> logEntries;
        return logEntries;
    }

    void assertionHandler(bool expr, const char* expr_str, const char* file, int line) {
        if (!expr) {
            log::error("Assertion failed: {} at {}:{}", expr_str, file, line);

            #if defined (DEBUG)
                std::abort();
            #endif
        }
    }

}