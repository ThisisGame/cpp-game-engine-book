//
// Created by captain on 2021/6/20.
//

#ifndef UNTITLED_KEYCODE_H
#define UNTITLED_KEYCODE_H



#define KEY_ACTION_UP          0x00
#define KEY_ACTION_DOWN            0x01
#define KEY_ACTION_REPEAT           0x02

/* The unknown key */
#define KEY_CODE_UNKNOWN            -1

#define MOUSE_BUTTON_1         0
#define MOUSE_BUTTON_2         1
#define MOUSE_BUTTON_3         2
#define MOUSE_BUTTON_4         3
#define MOUSE_BUTTON_5         4
#define MOUSE_BUTTON_6         5
#define MOUSE_BUTTON_7         6
#define MOUSE_BUTTON_8         7
#define MOUSE_BUTTON_LAST      MOUSE_BUTTON_8
#define MOUSE_BUTTON_LEFT      MOUSE_BUTTON_1
#define MOUSE_BUTTON_RIGHT     MOUSE_BUTTON_2
#define MOUSE_BUTTON_MIDDLE    MOUSE_BUTTON_3

/* Printable keys */
#define KEY_CODE_SPACE              32
#define KEY_CODE_APOSTROPHE         39  /* ' */
#define KEY_CODE_COMMA              44  /* , */
#define KEY_CODE_MINUS              45  /* - */
#define KEY_CODE_PERIOD             46  /* . */
#define KEY_CODE_SLASH              47  /* / */
#define KEY_CODE_0                  48
#define KEY_CODE_1                  49
#define KEY_CODE_2                  50
#define KEY_CODE_3                  51
#define KEY_CODE_4                  52
#define KEY_CODE_5                  53
#define KEY_CODE_6                  54
#define KEY_CODE_7                  55
#define KEY_CODE_8                  56
#define KEY_CODE_9                  57
#define KEY_CODE_SEMICOLON          59  /* ; */
#define KEY_CODE_EQUAL              61  /* = */
#define KEY_CODE_A                  65
#define KEY_CODE_B                  66
#define KEY_CODE_C                  67
#define KEY_CODE_D                  68
#define KEY_CODE_E                  69
#define KEY_CODE_F                  70
#define KEY_CODE_G                  71
#define KEY_CODE_H                  72
#define KEY_CODE_I                  73
#define KEY_CODE_J                  74
#define KEY_CODE_K                  75
#define KEY_CODE_L                  76
#define KEY_CODE_M                  77
#define KEY_CODE_N                  78
#define KEY_CODE_O                  79
#define KEY_CODE_P                  80
#define KEY_CODE_Q                  81
#define KEY_CODE_R                  82
#define KEY_CODE_S                  83
#define KEY_CODE_T                  84
#define KEY_CODE_U                  85
#define KEY_CODE_V                  86
#define KEY_CODE_W                  87
#define KEY_CODE_X                  88
#define KEY_CODE_Y                  89
#define KEY_CODE_Z                  90
#define KEY_CODE_LEFT_BRACKET       91  /* [ */
#define KEY_CODE_BACKSLASH          92  /* \ */
#define KEY_CODE_RIGHT_BRACKET      93  /* ] */
#define KEY_CODE_GRAVE_ACCENT       96  /* ` */
#define KEY_CODE_WORLD_1            161 /* non-US #1 */
#define KEY_CODE_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define KEY_CODE_ESCAPE             256
#define KEY_CODE_ENTER              257
#define KEY_CODE_TAB                258
#define KEY_CODE_BACKSPACE          259
#define KEY_CODE_INSERT             260
#define KEY_CODE_DELETE             261
#define KEY_CODE_RIGHT              262
#define KEY_CODE_LEFT               263
#define KEY_CODE_DOWN               264
#define KEY_CODE_UP                 265
#define KEY_CODE_PAGE_UP            266
#define KEY_CODE_PAGE_DOWN          267
#define KEY_CODE_HOME               268
#define KEY_CODE_END                269
#define KEY_CODE_CAPS_LOCK          280
#define KEY_CODE_SCROLL_LOCK        281
#define KEY_CODE_NUM_LOCK           282
#define KEY_CODE_PRINT_SCREEN       283
#define KEY_CODE_PAUSE              284
#define KEY_CODE_F1                 290
#define KEY_CODE_F2                 291
#define KEY_CODE_F3                 292
#define KEY_CODE_F4                 293
#define KEY_CODE_F5                 294
#define KEY_CODE_F6                 295
#define KEY_CODE_F7                 296
#define KEY_CODE_F8                 297
#define KEY_CODE_F9                 298
#define KEY_CODE_F10                299
#define KEY_CODE_F11                300
#define KEY_CODE_F12                301
#define KEY_CODE_F13                302
#define KEY_CODE_F14                303
#define KEY_CODE_F15                304
#define KEY_CODE_F16                305
#define KEY_CODE_F17                306
#define KEY_CODE_F18                307
#define KEY_CODE_F19                308
#define KEY_CODE_F20                309
#define KEY_CODE_F21                310
#define KEY_CODE_F22                311
#define KEY_CODE_F23                312
#define KEY_CODE_F24                313
#define KEY_CODE_F25                314
#define KEY_CODE_KP_0               320
#define KEY_CODE_KP_1               321
#define KEY_CODE_KP_2               322
#define KEY_CODE_KP_3               323
#define KEY_CODE_KP_4               324
#define KEY_CODE_KP_5               325
#define KEY_CODE_KP_6               326
#define KEY_CODE_KP_7               327
#define KEY_CODE_KP_8               328
#define KEY_CODE_KP_9               329
#define KEY_CODE_KP_DECIMAL         330
#define KEY_CODE_KP_DIVIDE          331
#define KEY_CODE_KP_MULTIPLY        332
#define KEY_CODE_KP_SUBTRACT        333
#define KEY_CODE_KP_ADD             334
#define KEY_CODE_KP_ENTER           335
#define KEY_CODE_KP_EQUAL           336
#define KEY_CODE_LEFT_SHIFT         340
#define KEY_CODE_LEFT_CONTROL       341
#define KEY_CODE_LEFT_ALT           342
#define KEY_CODE_LEFT_SUPER         343
#define KEY_CODE_RIGHT_SHIFT        344
#define KEY_CODE_RIGHT_CONTROL      345
#define KEY_CODE_RIGHT_ALT          346
#define KEY_CODE_RIGHT_SUPER        347
#define KEY_CODE_MENU               348

#define KEY_CODE_LAST               KEY_CODE_MENU

#endif //UNTITLED_KEYCODE_H
