#ifndef RTC_H
#define RTC_H
#include "result.h"
#include "stdbool.h"

#define UPDATE_PENDING_FLAG 0xDEADBEEF

bool rtc_is_update_pending(void);
void rtc_write_update_pending(void);
void rtc_clear_update_pending(void);

#endif /* RTC_H */
