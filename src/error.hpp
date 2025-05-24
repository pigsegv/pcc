#pragma once

#ifndef PCC_ERROR_HPP
#define PCC_ERROR_HPP

#define FORMAT_ERROR(x) "\033[31;1m" x "\033[0m"

void report_error(const char *path, const char *start, const char *loc,
                  const char *fmt, ...);

void report_warning(const char *start, const char *loc, const char *fmt, ...);

#endif // PCC_ERROR_HPP
