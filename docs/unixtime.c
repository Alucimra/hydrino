
uint32_t epochDays(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfMonth, uint8_t month, uint8_t year){
  year -= m <= 2;
  int16_t era = (year >= 0 ? year : year - 399) / 400;
  uint16_t yoe = (year - era * 400); // 0..399
  uint16_t doy = ((uint16_t)153*(month + (month > 2 ? -3 : 9)) + 2)/5 + dayOfMonth-1; // 0..365
  uint32_t doe = yoe * 365 + yoe/4 - yoe/100 + doy; // 0..146096
  // Note: remove the 719468 to set epoch to 2000-03-01 instead of 1970-01-01
  return (int32_t)era * 146097 + doe - 719468);
}

uint32_t dateToUnixTimestamp(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfMonth, uint8_t month, uint8_t year){

}

void dateFromUnixTimestamp(uint32_t utime, uint8_t *second, uint8_t *minute, uint8_t *hour, uint8_t *dayOfMonth, uint8_t *month, uint8_t *year){

}
