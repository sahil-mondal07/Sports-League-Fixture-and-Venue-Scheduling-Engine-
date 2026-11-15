/* ------------------------------------------------------------
 * GUI Module for Graphify Sports League Scheduler
 * Provides a simple command-line interface for schedule validation
 * ------------------------------------------------------------ */
#ifndef GUI_H
#define GUI_H

#include "scheduling/validator.h"

/* ------------------------------------------------------------
 * Display welcome banner
 * ------------------------------------------------------------ */
void gui_welcome_banner(void);

/* ------------------------------------------------------------
 * Display validation menu
 * ------------------------------------------------------------ */
void gui_display_menu(void);

/* ------------------------------------------------------------
 * Display validation results report
 * ------------------------------------------------------------ */
void gui_display_report(const ValidationReport* report);

/* ------------------------------------------------------------
 * Run quick test with sample data
 * ------------------------------------------------------------ */
int gui_quick_test(void);

/* --------------------------------------------------------- ---
 * Main GUI loop
 * ------------------------------------------------------------ */
int gui_main_loop(void);

#endif /* GUI_H */