#pragma once

#ifndef PCC_ERROR_HPP
#define PCC_ERROR_HPP

namespace pcc {

void report_error(const char *path, const char *start, const char *loc, const char *msg);

void report_warning(const char *start, const char *loc, const char *msg);

}

#endif // PCC_ERROR_HPP
