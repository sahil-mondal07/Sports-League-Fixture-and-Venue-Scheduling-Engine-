#include "common.h"

// Convert date to days since epoch (Jan 1, 1970)
int date_to_days(Date* date) {
    int days = 0;

    // Days in each month (non-leap year)
    int month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Add years
    for (int y = 1970; y < date->year; y++) {
        days += 365;
        // Leap year check
        if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) {
            days++;
        }
    }

    // Add months
    for (int m = 0; m < date->month - 1; m++) {
        days += month_days[m];
        // Add leap day if February and leap year
        if (m == 1 && ((date->year % 4 == 0 && date->year % 100 != 0) || (date->year % 400 == 0))) {
            days++;
        }
    }

    // Add days
    days += date->day;

    return days;
}

// Convert days since epoch to date
Date days_to_date(int days) {
    Date date = {1, 1, 1970};
    int month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Calculate year
    while (1) {
        int year_days = 365;
        if ((date.year % 4 == 0 && date.year % 100 != 0) || (date.year % 400 == 0)) {
            year_days = 366;
        }

        if (days > year_days) {
            days -= year_days;
            date.year++;
        } else {
            break;
        }
    }

    // Calculate month
    for (int m = 0; m < 12; m++) {
        int m_days = month_days[m];
        if (m == 1 && ((date.year % 4 == 0 && date.year % 100 != 0) || (date.year % 400 == 0))) {
            m_days = 29;
        }

        if (days > m_days) {
            days -= m_days;
            date.month++;
        } else {
            break;
        }
    }

    date.day = days;
    return date;
}

// Calculate days between two dates
int days_between(Date* d1, Date* d2) {
    return date_to_days(d2) - date_to_days(d1);
}

// Validate date
int is_valid_date(Date* date) {
    if (date->year < 1970 || date->year > 2100) return FALSE;
    if (date->month < 1 || date->month > 12) return FALSE;

    int month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Adjust for leap year
    if (date->month == 2 && ((date->year % 4 == 0 && date->year % 100 != 0) || (date->year % 400 == 0))) {
        month_days[1] = 29;
    }

    if (date->day < 1 || date->day > month_days[date->month - 1]) return FALSE;

    return TRUE;
}

// Print date in DD/MM/YYYY format
void print_date(Date* date) {
    printf("%02d/%02d/%d", date->day, date->month, date->year);
}

// Add days to a date
Date add_days(Date* date, int days) {
    int total_days = date_to_days(date) + days;
    return days_to_date(total_days);
}
