#include "update_flag.h"
#include "main.h"

extern RTC_HandleTypeDef hrtc;

static void init_rtc(void) {
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();
}

bool rtc_is_update_pending(void) {
  init_rtc();
  if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) == UPDATE_PENDING_FLAG) {
    return true;
  }
  return false;
}

void rtc_write_update_pending(void) {
  init_rtc();
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, UPDATE_PENDING_FLAG);
}

void rtc_clear_update_pending(void) {
  init_rtc();
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0x00000000);
}
