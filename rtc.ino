
int rtc_get()
{
  bool h12;
  bool hPM;
  CurrentTime.Seconds = myRTC.getSecond();
  CurrentTime.Minutes = myRTC.getMinute();
  CurrentTime.Hours = myRTC.getHour(h12, hPM);
  CurrentDate.WeekDay = myRTC.getDoW();
  return 0;
}

void SaveToRTC() {
  Serial.println("Save To RTC");
  Serial.print(prevTimeState.hour); Serial.print(":");
  Serial.print(prevTimeState.minute); Serial.print("  ");
  Serial.println(prevTimeState.dayOfWeek + 1); 
  
  myRTC.setHour(prevTimeState.hour);
  myRTC.setMinute(prevTimeState.minute);
  myRTC.setSecond(0);
  myRTC.setDoW(prevTimeState.dayOfWeek + 1);

  
  bool h12;
  bool hPM;
  Serial.print(myRTC.getHour(h12, hPM)); Serial.print(":");
  Serial.print(myRTC.getMinute()); Serial.print("  ");
  Serial.println(myRTC.getDoW()); 
}
