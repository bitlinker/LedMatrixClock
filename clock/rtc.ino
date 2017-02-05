




//
//#define DS3231_I2C_ADDRESS 0x68
//// 0x57  - mem
//
//byte decToBcd(byte val)
//{
//  return( (val/10*16) + (val%10) );
//}
//
//byte bcdToDec(byte val)
//{
//  return( (val/16*10) + (val%16) );
//}
//
//void rtcStart()
//{
// Wire.begin();
// rtcSetTime(30,42,21,4,26,11,14);
//}
//
//void rtcSetTime(
//  byte second,
//  byte minute,
//  byte hour,
//  byte dayOfWeek,
//  byte dayOfMonth,
//  byte month,
//  byte year)
//{
//  Wire.beginTransmission(DS3231_I2C_ADDRESS);
//  Wire.write(0); // set next input to start at the seconds register
//  Wire.write(decToBcd(second)); // set seconds
//  Wire.write(decToBcd(minute)); // set minutes
//  Wire.write(decToBcd(hour)); // set hours
//  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
//  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
//  Wire.write(decToBcd(month)); // set month
//  Wire.write(decToBcd(year)); // set year (0 to 99)
//  Wire.endTransmission();
//}
//
//void rtcReadTime(
//  byte *second,
//  byte *minute,
//  byte *hour,
//  byte *dayOfWeek,
//  byte *dayOfMonth,
//  byte *month,
//  byte *year)
//{
//  Wire.beginTransmission(DS3231_I2C_ADDRESS);
//  Wire.write(0); // set DS3231 register pointer to 00h
//  Wire.endTransmission();
//  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
//  // request seven bytes of data from DS3231 starting from register 00h
//  *second = bcdToDec(Wire.read() & 0x7f);
//  *minute = bcdToDec(Wire.read());
//  *hour = bcdToDec(Wire.read() & 0x3f);
//  *dayOfWeek = bcdToDec(Wire.read());
//  *dayOfMonth = bcdToDec(Wire.read());
//  *month = bcdToDec(Wire.read());
//  *year = bcdToDec(Wire.read());
//}
