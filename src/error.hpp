#pragma once

#ifndef PCC_ERROR_HPP
#define PCC_ERROR_HPP

#define FORMAT_ERROR(x) "\033[31;1m" x "\033[0m"
#define EXIT_AND_ERR(path, start, loc, fmt, ...)                  \
  (report_error(path, start, loc, fmt __VA_OPT__(,) __VA_ARGS__), \
   std::exit(EXIT_FAILURE), 0)

void report_error(const char *path, const char *start, const char *loc,
                  const char *fmt, ...);

void report_warning(const char *start, const char *loc, const char *fmt, ...);

#endif // PCC_ERROR_HPP
