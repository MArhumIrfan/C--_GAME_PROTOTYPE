#pragma once

#include <string>
#include <vector>

// All ASCII-art sprites used by the renderer. `inline` (C++17) so these
// can be defined here and included from multiple translation units without
// violating the one-definition rule.

inline const std::vector<std::string> spriteBread = {
        "          .-\"\"\"\"\"\"\"\"\"\"\"\"-.          ",
        "        .-'                '-.        ",
        "      .'                      '.      ",
        "     /    .--.                 \\      ",
        "    ;    /    \\    .           ;      ",
        "    |   |  ()  |       .       |      ",
        "    |    \\____/   .           |       ",
        "    ;       .       .   *     ;       ",
        "     \\   .      *             /       ",
        "      '.                    .'        ",
        "        '-.______________.-'          ",
        "           .  *    .  *  .            "
    };

inline const std::vector<std::string> spriteMeds = {
        "                         .-~~~~~-.                      ",
        "                       .'  .---.  '.                    ",
        "                      /   /     \\   \\                   ",
        "                     |   |  _ _  |   |                  ",
        "                     |   | |   | |   |                  ",
        "                     |   | |___| |   |                  ",
        "                     |    \\_____/    |                  ",
        "                     |      |||      |                  ",
        "                 ____|______|||______|____              ",
        "              .-'                         '-.           ",
        "            .'       .------------- .         '.        ",
        "           /        /               \\           \\       ",
        "          /        /                 \\           \\      ",
        "         ;        |                  |           ;      ",
        "         |        |                  |           |      ",
        "         |        |   .----------.   |           |      ",
        "         |        |   |          |   |           |      ",
        "         |        |   |  1897    |   |           |      ",
        "         |        |   '----------'   |           |      ",
        "         |        |                  |           |      ",
        "         |        |                  |           |      ",
        "         ;        |                  |           ;      ",
        "          \\       |   . . . . . .    |          /       ",
        "           \\      '-----._____.-----'         /         ",
        "            '.            .  .              .'          ",
        "              '-._      .      .        _.-'            ",
        "                  '----.__________.-----'               ",
        "                    _/    /  \\    \\_                    ",
        "                  _/_____/____\\_____\\_                  ",
        "                 /   .     ||     .   \\                 ",
        "                /  .    *  ||  .     . \\                ",
        "               |      .    ||    *      |               ",
        "               |  *        ||       .   |               ",
        "                \\__________||__________/                ",
        "                 \\         ||         /                 ",
        "                  '--------''--------'                  "
    };

inline const std::vector<std::string> spritePebble = {
        "    _----------_,                 ",
        "    ,\"__         _-:,             ",
        "   /    \"\"--_--\"\"...:\\            ",
        "  /         |.........\\           ",
        " /          |..........\\          ",
        "/,         _'_........./:         ",
        "! -,    _-\"   \"-_... ,;;:         ",
        "\\   -_-\"         \"-_/;;;;         ",
        " \\   \\             /;;;;'         ",
        "  \\   \\           /;;;;           ",
        "   '.  \\         /;;;'            ",
        "     \"-_\\_______/;;'              "
    };

inline const std::vector<std::string> spriteKey = {
        "   88888888     ",
        "  88      88    ",
        "  88      88    ",
        "   88888888     ",
        "      ||        ",
        "      ||        ",
        "      ||--'     ",
        "      ||        ",
        "      ||--'     ",
        "      ||        "
    };

inline const std::vector<std::string> spriteMist0 = {
        "               __,aaPPPPPPPPaa,__               ",
        "           ,adP\"\"\"'          `\"\"Yb,_            ",
        "        ,adP'                     `\"Yb,         ",
        "      ,dP'     ,aadPP\"\"\"\"\"YYba,_     `\"Y,       ",
        "     ,P'    ,aP\"'            `\"\"Ya,     \"Y,     ",
        "    ,P'    aP'     _________     `\"Ya    `Yb,   ",
        "   ,P'    d\"    ,adP\"\"\"\"\"\"\"\"Yba,    `Y,    \"Y,  ",
        "  ,d'   ,d'   ,dP\"            `Yb,   `Y,    `Y, ",
        "  d'   ,d'   ,d'    ,dP\"\"Yb,    `Y,   `Y,    `b ",
        "  8    d'    d'   ,d\"      \"b,   `Y,   `8,    Y,",
        "  8    8     8    d'    _   `Y,   `8    `8    `b",
        "  8    8     8    8     8    `8    8     8     8",
        "  8    Y,    Y,   `b, ,aP     P    8    ,P     8",
        "  I,   `Y,   `Ya    \"\"\"\"     d'   ,P    d\"    ,P",
        "  `Y,   `8,    `Ya         ,8\"   ,P'   ,P'    d'",
        "   `Y,   `Ya,    `Ya,,__,,d\"'   ,P'   ,P\"    ,P ",
        "    `Y,    `Ya,     `\"\"\"\"'     ,P'   ,d\"    ,P' ",
        "     `Yb,    `\"Ya,_          ,d\"    ,P'    ,P'  ",
        "       `Yb,      \"\"YbaaaaaadP\"     ,P'    ,P'   ",
        "         `Yba,                   ,d'    ,dP'    ",
        "            `\"Yba,__       __,adP\"     dP\"      ",
        "                `\"\"\"\"\"\"\"\"\"\"\"\"\"'                 "
    };

inline const std::vector<std::string> spriteMist1 = {
        "                                                ",
        "               __,aaPPPPPPPPaa,__               ",
        "           ,adP\"\"\"'          `\"\"Yb,_            ",
        "        ,adP'                     `\"Yb,         ",
        "      ,dP'     ,aadPP\"\"\"\"\"YYba,_     `\"Y,       ",
        "     ,P'    ,aP\"'            `\"\"Ya,     \"Y,     ",
        "    ,P'    aP'     _________     `\"Ya    `Yb,   ",
        "   ,P'    d\"    ,adP\"\"\"\"\"\"\"\"Yba,    `Y,    \"Y,  ",
        "  ,d'   ,d'   ,dP\"            `Yb,   `Y,    `Y, ",
        "  d'   ,d'   ,d'    ,dP\"\"Yb,    `Y,   `Y,    `b ",
        "  8    d'    d'   ,d\"      \"b,   `Y,   `8,    Y,",
        "  8    8     8    d'    _   `Y,   `8    `8    `b",
        "  8    8     8    8     8    `8    8     8     8",
        "  8    Y,    Y,   `b, ,aP     P    8    ,P     8",
        "  I,   `Y,   `Ya    \"\"\"\"     d'   ,P    d\"    ,P",
        "  `Y,   `8,    `Ya         ,8\"   ,P'   ,P'    d'",
        "   `Y,   `Ya,    `Ya,,__,,d\"'   ,P'   ,P\"    ,P ",
        "    `Y,    `Ya,     `\"\"\"\"'     ,P'   ,d\"    ,P' ",
        "     `Yb,    `\"Ya,_          ,d\"    ,P'    ,P'  ",
        "       `Yb,      \"\"YbaaaaaadP\"     ,P'    ,P'   ",
        "         `Yba,                   ,d'    ,dP'    ",
        "            `\"Yba,__       __,adP\"     dP\"      "
    };

// Populated at runtime by Game::initializeSprites() (padded from the raw
// stalker art to a uniform rectangular block) - not const, and not
// initialized here.
inline std::vector<std::string> spriteStalker0;
inline std::vector<std::string> spriteStalker1;

// NEW: Statue Sprites
inline std::vector<std::string> spriteStatue;
inline std::vector<std::string> spriteStatueJumpscare;
