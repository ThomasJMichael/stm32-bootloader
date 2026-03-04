#ifndef RESULT_H
#define RESULT_H
typedef enum {
  RESULT_OK = 0,
  RESULT_ERROR = -1,
  RESULT_ERROR_INVALID_ARGUMENT = -2,
  RESULT_ERROR_NULL_POINTER = -3,
  RESULT_ERROR_SIZE_ZERO = -4,
  RESULT_ERROR_ALIGNMENT = -5,
  RESULT_ERROR_WRITE_FAILURE = -6,
} result_t;

#define RESULT_IS_OK(res) ((res) >= RESULT_OK)
#define RESULT_IS_ERROR(res) ((res) < RESULT_ERROR)

#define RESULT_OK_OR_RETURN(x)                                                 \
  do {                                                                         \
    result_t res = (x);                                                        \
    if (RESULT_IS_ERROR(res)) {                                                \
      return RESULT_ERROR;                                                     \
    }                                                                          \
  } while (0)

static inline const char *result_to_string(result_t res) {
  switch (res) {
  case RESULT_OK:
    return "OK";
  case RESULT_ERROR:
    return "ERROR";
  case RESULT_ERROR_INVALID_ARGUMENT:
    return "ERROR_INVALID_ARGUMENT";
  case RESULT_ERROR_NULL_POINTER:
    return "ERROR_NULL_POINTER";
  case RESULT_ERROR_SIZE_ZERO:
    return "ERROR_SIZE_ZERO";
  case RESULT_ERROR_ALIGNMENT:
    return "ERROR_ALIGNMENT";
  case RESULT_ERROR_WRITE_FAILURE:
    return "ERROR_WRITE_FAILURE";
  default:
    return "UNKNOWN";
  }
}
#endif
