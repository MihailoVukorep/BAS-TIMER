#include "colors.h"
#include "debug.h"
#include "globals.h"
#include "reqworker.h"
#include "spinners.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char* weather[] = {
    CTEXT_FG(196, ""),
    CTEXT_FG(208, ""),
    CTEXT_FG(208, "󰖨"),
    CTEXT_FG(214, ""),
    CTEXT_FG(75, ""),
    CTEXT_FG(81, ""),
    CTEXT_FG(87, "")};
char* temp_to_emoji(double temp) {
    if (temp > 40) { return weather[0]; }                //  super hot
    if (temp >= 30 && temp <= 39) { return weather[1]; } //  really hot
    if (temp >= 25 && temp <= 29) { return weather[2]; } // 󰖨 hot
    if (temp >= 15 && temp <= 24) { return weather[3]; } //  mild
    if (temp >= 5 && temp <= 14) { return weather[4]; }  //  cold
    if (temp >= -5 && temp <= 4) { return weather[5]; }  //  really cold
    return weather[6];                                   //  super cold
}

char* clock_hours[] = {"", "", "", "", "", "", "", "", "", "", "", ""};
char* hour_to_clock(int hour) {
    return clock_hours[hour % 12];
}

char* hour_to_emoji(int hour) {
    if (hour >= 5 && hour <= 7) return get_frame(&spinner_sunrise, 1); // Sunrise
    if (hour >= 7 && hour < 12) return "";                          // Morning
    if (hour >= 12 && hour < 17) return "󰖙";                        // Afternoon
    if (hour >= 17 && hour < 19) return get_frame(&spinner_sunset, 1); // Sunset
    if (hour >= 19 && hour < 21) return "";                         // Evening
    return "󰖔";                                                     // Night
}

int hour_to_color(int hour) {
    if (hour >= 5 && hour < 7) return 214;   // Orange (Sunrise)
    if (hour >= 7 && hour < 12) return 220;  // Bright Yellow (Morning)
    if (hour >= 12 && hour < 17) return 208; // Deep Orange (Afternoon)
    if (hour >= 17 && hour < 19) return 202; // Red-Orange (Sunset)
    if (hour >= 19 && hour < 21) return 99;  // Purple-Pink (Evening)
    return 33;                               // Deep Blue (Night)
}

char* draw_heat(int isOn) {
    return isOn ? ctext_fg(COLOR_ON, get_frame(&spinner_heat, 1)) : ctext_fg(COLOR_OFF, "󱪯");
}

char* draw_pump_bars(int isOn) {
    return isOn ? ctext_fg_con(COLOR_ON, get_frame(&spinner_bars, 0)) : ctext_fg(COLOR_OFF, "");
}

char* bool_to_check(int isOn) {
    return isOn ? ctext_fg(COLOR_ON, "") : ctext_fg(COLOR_OFF, "");
}

#define TERM_BUFFER_SIZE 1024 * 2
char g_term_buffer[TERM_BUFFER_SIZE] = {0};

extern int g_auto_timer;
extern int g_auto_timer_started;
extern double g_auto_timer_seconds;
extern double g_auto_timer_seconds_elapsed;
extern time_t g_history_mode_time_on;
extern int g_auto_gas;
extern char g_auto_timer_status[];
extern char g_auto_gas_status[];

// clang-format off
#define HEADER_BUFFER_SIZE 1024
int draw_ui(struct bas_info info, int is_sending, int errors) {

    // make header
    char* emoji_basic = ctext_fg(228, get_frame(&spinner_basic, 1));
    char* emoji_light = ctext_fg(228, get_frame(&spinner_lights, 1));
    char* emoji_send  = is_sending ? CTEXT_FG(211, "") : " ";

    char full_time_header_raw[HEADER_BUFFER_SIZE];
    char* current_time = get_current_time();
    int hour           = get_current_hour();
    char* emoji_clock  = hour_to_clock(hour);
    char* emoji_dayhr  = hour_to_emoji(hour);
    snprintf(full_time_header_raw, HEADER_BUFFER_SIZE, "%s %s %s", emoji_clock, current_time, emoji_dayhr);
    free(current_time);

    char* full_time_header = ctext_fg(hour_to_color(hour), full_time_header_raw);

    char* ip_raw       = get_local_ips();
    char* ip           = ctext_fg(errors ? COLOR_OFF : COLOR_ON, ip_raw);
    free(ip_raw);


    char* label_Tspv     = CTEXT_FG(213 , "Outside 󱇜"); char* Tspv     = temp_to_ctext_bg_con(info.Tspv);
    char* label_Tsolar   = CTEXT_FG(230 , "  Solar "); char* Tsolar   = temp_to_ctext_bg_con(info.Tsolar);
    char* label_Tsobna   = CTEXT_FG( 76 , "   Room "); char* Tsobna   = temp_to_ctext_bg_con(info.Tsobna);
    char* label_Tzadata  = CTEXT_FG(154 , "    Set "); char* Tzadata  = temp_to_ctext_bg_con(info.Tzadata);
    char* label_Tmax     = CTEXT_FG(214 , "    Max "); char* Tmax     = temp_to_ctext_bg_con(info.Tmax);
    char* label_Tmid     = CTEXT_FG(220 , "    Mid "); char* Tmid     = temp_to_ctext_bg_con(info.Tmid);
    char* label_Tmin     = CTEXT_FG(226 , "    Min "); char* Tmin     = temp_to_ctext_bg_con(info.Tmin);
    char* label_Tfs      = CTEXT_FG(110 , "  Circ. "); char* Tfs      = temp_to_ctext_bg_con(info.Tfs);
    char* label_Thottest = CTEXT_FG(196 , "Hottest 󰈸"); char* Thottest = temp_to_ctext_bg_con(info.Thottest);
    char* label_Tcoldest = CTEXT_FG( 51 , "Coldest "); char* Tcoldest = temp_to_ctext_bg_con(info.Tcoldest);

    char* moving_emoji_heat  = ctext_fg(212 , info.StatusPumpe6 ? get_frame(&spinner_heat_pump, 1) : "󱩃");
    char* moving_emoji_gas   = ctext_fg(203 , info.StatusPumpe4 ? get_frame(&spinner_fire,      1) : "󰙇");
    char* moving_emoji_circ  = ctext_fg(168 , info.StatusPumpe3 ? get_frame(&spinner_circle,    1) : "");
    char* moving_emoji_solar = ctext_fg(224 , info.StatusPumpe7 ? get_frame(&spinner_solar,     1) : "");
    char* moving_emoji_elec  = ctext_fg( 78 , info.StatusPumpe5 ? get_frame(&spinner_lightning, 1) : "󰠠");

    char* label_mode      = CTEXT_FG(222 , "   Mode 󱪯"); char* mode      = draw_heat(info.mod_rada);
    char* label_regime    = CTEXT_FG(192 , " Regime 󱖫"); char* regime    = cnum_fg(192, info.mod_rezim);
    char* label_heat      = CTEXT_FG(212 , "   Heat ");  char* heat      = draw_pump_bars(info.StatusPumpe6);
    char* label_gas       = CTEXT_FG(203 , "    Gas ");  char* gas       = draw_pump_bars(info.StatusPumpe4);
    char* label_circ      = CTEXT_FG(168 , "  Circ. ");  char* circ      = draw_pump_bars(info.StatusPumpe3);
    char* label_solar     = CTEXT_FG(224 , "  Solar ");  char* solar     = draw_pump_bars(info.StatusPumpe7);
    char* label_elec      = CTEXT_FG( 78 , "  Elec. ");  char* elec      = draw_pump_bars(info.StatusPumpe5);
    char* label_min_check = CTEXT_FG( 87 , " Min < 45"); char* min_check = bool_to_check(info.TminLT);
    char* label_mid_check = CTEXT_FG(208 , "Mid >= 60"); char* mid_check = bool_to_check(info.TmidGE);

    char* emoji_check = ctext_fg(82, info.TmidGE ? get_frame(&spinner_check, 1) : " ");
    char* emoji_warn  = ctext_fg(51, info.TminLT ? get_frame(&spinner_snow, 1) : " ");

    char* emoji_eye1  = ctext_fg(COLOR_ON, g_auto_timer ? get_frame(&spinner_eye_left, 0) : " ");
    char* emoji_timer = g_auto_timer_started ? ctext_fg(COLOR_ON, get_frame(&spinner_clock, 0)) : ctext_fg(COLOR_OFF, g_auto_timer ? "󱎫" : " ");
    char* emoji_eye2  = ctext_fg(COLOR_ON, g_auto_gas   ? get_frame(&spinner_eye_left, 0) : " ");

    char* emoji_reye1 = g_auto_timer ?  ctext_fg(COLOR_ON, get_frame(&spinner_eye_right, 0)) : ctext_fg(COLOR_OFF, "");
    char* emoji_reye2 = g_auto_gas   ?  ctext_fg(COLOR_ON, get_frame(&spinner_eye_right, 0)) : ctext_fg(COLOR_OFF, "");

    if (g_auto_timer_started) {

        time_t current_time;
        time(&current_time);
        g_auto_timer_seconds_elapsed = difftime(current_time, g_history_mode_time_on);
        snprintf(g_auto_timer_status, STATUS_BUFFER_SIZE, "%.0f/%.0f", g_auto_timer_seconds_elapsed, g_auto_timer_seconds);

    }

    int bytes = snprintf(g_term_buffer, TERM_BUFFER_SIZE,
             "%s%s%s %s %s\n"
             "%s  %s %s %s  %s %s%s\n"
             "%s  %s %s %s  %s\n"
             "%s  %s %s %s%s  %s\n"
             "%s  %s %s %s%s  %s %s\n"
             "%s  %s   %s%s  %s\n"
             "%s  %s %s %s%s  %s\n"
             "%s  %s %s %s%s  %s\n"
             "%s  %s   %s  %s\n"
             "%s  %s   %s  %s\n"
             "%s  %s\n"
             "%s󱪯 %-37s\n"
             "%s󰙇 %-37s\n",
             emoji_basic, emoji_light, emoji_send, full_time_header, ip,
             label_Tspv,     Tspv,    temp_to_emoji(info.Tspv),    label_mode     ,                     mode,   emoji_eye1, emoji_timer,
             label_Tsolar,   Tsolar,  temp_to_emoji(info.Tsolar),  label_regime   ,                     regime,
             label_Tsobna,   Tsobna,  temp_to_emoji(info.Tsobna),  label_heat     , moving_emoji_heat,  heat,
             label_Tzadata,  Tzadata, temp_to_emoji(info.Tzadata), label_gas      , moving_emoji_gas,   gas,    emoji_eye2,
             label_Tmax,     Tmax,                                   label_circ     , moving_emoji_circ,  circ,
             label_Tmid,     Tmid,    emoji_check,                   label_solar    , moving_emoji_solar, solar,
             label_Tmin,     Tmin,    emoji_warn,                    label_elec     , moving_emoji_elec,  elec,
             label_Tfs,      Tfs,                                    label_min_check,                     min_check,
             label_Thottest, Thottest,                               label_mid_check,                     mid_check,
             label_Tcoldest, Tcoldest,
             emoji_reye1, g_auto_timer_status,
             emoji_reye2, g_auto_gas_status
             );


    
    

    free(emoji_basic); free(emoji_light); free(full_time_header); free(ip);
    free(Tspv);                                               free(mode);   free(emoji_eye1); free(emoji_timer);
    free(Tsolar);                                             free(regime);
    free(Tsobna);                  free(moving_emoji_heat);   free(heat);
    free(Tzadata);                 free(moving_emoji_gas);    free(gas);    free(emoji_eye2);
    free(Tmax);                    free(moving_emoji_circ);   free(circ);
    free(Tmid); free(emoji_check); free(moving_emoji_solar);  free(solar);
    free(Tmin); free(emoji_warn);  free(moving_emoji_elec);   free(elec);
    free(Tfs);                                                free(min_check);
    free(Thottest);                                           free(mid_check);
    free(Tcoldest);
    free(emoji_reye1);
    free(emoji_reye2);

    spin_spinner(&spinner_circle);
    spin_spinner(&spinner_eye_left);
    spin_spinner(&spinner_eye_right);
    spin_spinner(&spinner_bars);
    spin_spinner(&spinner_clock);

    DPL("===[ OUTPUT BEGIN ]===\n");
    printf("%s", g_term_buffer);
    DPL("===[ OUTPUT END ]===\n");
    D(printf("OUTPUT LEN: %d\n", bytes));
    return bytes;
}
// clang-format on
