
#ifndef __CLOCK_H_
#define __CLOCK_H_

void setClock(void);
bool GetCurrentTime(int &Month, int &Day, int &Hour, int &Minute);
String timeToString (time_t time);

#endif //__CLOCK_H_
