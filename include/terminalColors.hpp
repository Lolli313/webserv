#ifndef TERM_COLORS_HPP
#define TERM_COLORS_HPP

/* ===== Screen control ===== */
#define CLEAR          "\033[2J\033[H"

/* ===== Standard colors ===== */
#define BLACK          "\x1b[30m"
#define RED            "\x1b[31m"
#define GREEN          "\x1b[32m"
#define YELLOW         "\x1b[33m"
#define BLUE           "\x1b[34m"
#define PURPLE         "\x1b[35m"
#define CYAN           "\x1b[36m"
#define WHITE          "\x1b[37m"

/* ===== Bright colors ===== */
#define BLACK_BRIGHT   "\x1b[90m"
#define RED_BRIGHT     "\x1b[91m"
#define GREEN_BRIGHT   "\x1b[92m"
#define YELLOW_BRIGHT  "\x1b[93m"
#define BLUE_BRIGHT    "\x1b[94m"
#define PURPLE_BRIGHT  "\x1b[95m"
#define CYAN_BRIGHT    "\x1b[96m"
#define WHITE_BRIGHT   "\x1b[97m"

/* ===== Extended / 256-color ===== */
#define PINK           "\x1b[38;5;205m"
#define HOT_PINK       "\x1b[38;5;198m"
#define MAGENTA        "\x1b[38;5;201m"
#define TEAL           "\x1b[38;5;37m"
#define LIME           "\x1b[38;5;154m"
#define GOLD           "\x1b[38;5;220m"
#define SKY_BLUE       "\x1b[38;5;117m"

/* ===== RGB true color ===== */
#define ORANGE         "\x1b[38;2;255;165;0m"
#define DARK_ORANGE    "\x1b[38;2;255;140;0m"
#define LIGHT_BLUE     "\x1b[38;2;173;216;230m"
#define MINT           "\x1b[38;2;152;255;152m"
#define LAVENDER       "\x1b[38;2;230;230;250m"
#define CORAL          "\x1b[38;2;255;127;80m"

/* ===== Grayscale ===== */
#define GRAY           "\x1b[38;5;245m"
#define DARK_GRAY      "\x1b[38;5;240m"
#define LIGHT_GRAY     "\x1b[38;5;250m"

/* ===== Reset / defaults ===== */
#define RESET          "\x1b[0m"
#define DEFAULT        "\x1b[39m"

#endif