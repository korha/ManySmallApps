Conversion time between Unix and human mode

Run parameters:
[/tab:date] - Activate Date -> UnixTime tab
[/msec:1] - Use milliseconds
[/format:%format%] - DateTime format

Format consists of:
d - The day as number without a leading zero (1 to 31)
dd - The day as number with a leading zero (01 to 31)
ddd - The abbreviated localized day name (e.g. 'Mon' to 'Sun')
dddd - The long localized day name (e.g. 'Monday' to 'Sunday')
M - The month as number without a leading zero (1-12)
MM - The month as number with a leading zero (01-12)
MMM - The abbreviated localized month name (e.g. 'Jan' to 'Dec')
MMMM - The long localized month name (e.g. 'January' to 'December')
yy - The year as two digit number (00-99)
yyyy - The year as four digit number
h - The hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display)
hh - The hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)
H - The hour without a leading zero (0 to 23, even with AM/PM display)
HH - The hour with a leading zero (00 to 23, even with AM/PM display)
m - The minute without a leading zero (0 to 59)
mm - The minute with a leading zero (00 to 59)
s - The second without a leading zero (0 to 59)
ss - The second with a leading zero (00 to 59)
z - The milliseconds without leading zeroes (0 to 999)
zzz - The milliseconds with leading zeroes (000 to 999)
AP or A - Interpret as an AM/PM time. AP must be either "AM" or "PM"
ap or a - Interpret as an AM/PM time. ap must be either "am" or "pm"

e.g.:
yyyy.MM.dd hh:mm:ss - 2014-01-01 12:15:00 (1 January 2014, 12h 15min)
