/* Chemfiles, an efficient IO library for chemistry file formats
* Copyright (C) 2015 Guillaume Fraux
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
// clang-format off

#include <cassert>

#include "chemfiles.h"
#include "chemfiles/Logger.hpp"
#include "chemfiles/capi.hpp"
using namespace chemfiles;

const std::map<int, std::string> ERROR_MESSAGES = {
    {CHFL_SUCCESS, "operation was sucessfull"},
    {CHFL_MEMORY_ERROR, "memory allocation error."},
    {CHFL_FILE_ERROR, "system error while reading a file"},
    {CHFL_FORMAT_ERROR, "error while parsing a file"},
    {CHFL_SELECTION_ERROR, "error in selection parsing or evaluation"},
    {CHFL_GENERIC_ERROR, "unknown error from chemfiles library"},
    {CHFL_CXX_ERROR, "error from the C++ standard library"},
};

std::string chemfiles::CAPI_LAST_ERROR = "";

const char* chfl_version(void) {
    return CHEMFILES_VERSION;
}

const char* chfl_strerror(int code) {
    auto message = ERROR_MESSAGES.find(code);
    if (message != ERROR_MESSAGES.end()) {
        return message->second.c_str();
    } else {
        return "";
    }
}

const char* chfl_last_error(void) {
    return CAPI_LAST_ERROR.c_str();
}

int chfl_clear_errors(void) {
    CHFL_ERROR_CATCH(
        CAPI_LAST_ERROR = "";
    )
}

int chfl_loglevel(chfl_log_level_t* level) {
    assert(level != nullptr);
    CHFL_ERROR_CATCH(
        *level = static_cast<chfl_log_level_t>(Logger::level());
    )
}

int chfl_set_loglevel(chfl_log_level_t level) {
    CHFL_ERROR_CATCH(
        Logger::set_level(static_cast<LogLevel>(level));
    )
}

int chfl_logfile(const char* file) {
    assert(file != nullptr);
    CHFL_ERROR_CATCH(
        Logger::to_file(std::string(file));
    )
}

int chfl_log_stdout(void) {
    CHFL_ERROR_CATCH(
        Logger::to_stdout();
    )
}

int chfl_log_stderr(void) {
    CHFL_ERROR_CATCH(
        Logger::to_stderr();
    )
}

int chfl_log_silent(void) {
    CHFL_ERROR_CATCH(
        Logger::silent();
    )
}

int chfl_log_callback(chfl_logging_cb callback) {
    CHFL_ERROR_CATCH(
        Logger::callback([callback](LogLevel level, const std::string& message) {
            callback(static_cast<chfl_log_level_t>(level), message.c_str());
        });
    )
}
