/*
 * Mr. 4th Dimention - Allen Webster
 *
 * 12.12.2014
 *
 * Application Layer for 4coder
 *
 */

// TOP

#ifndef FRED_H
#define FRED_H

struct Application_Memory{
    void *vars_memory;
    i32 vars_memory_size;
    void *target_memory;
    i32 target_memory_size;
};

#define KEY_INPUT_BUFFER_SIZE 4
#define KEY_INPUT_BUFFER_DSIZE (KEY_INPUT_BUFFER_SIZE << 1)

enum Key_Control{
	CONTROL_KEY_SHIFT,
	CONTROL_KEY_CONTROL,
	CONTROL_KEY_ALT,
    CONTROL_KEY_CAPS,
	// always last
	CONTROL_KEY_COUNT
};

struct Key_Event_Data{
	u8 keycode;
	u8 character;
	u8 character_no_caps_lock;
    
	b8 modifiers[CONTROL_KEY_COUNT];
};

struct Key_Input_Data{
	Key_Event_Data press[KEY_INPUT_BUFFER_SIZE];
	Key_Event_Data hold[KEY_INPUT_BUFFER_SIZE];
	i32 press_count;
    i32 hold_count;
};

struct Key_Summary{
    i32 count;
    Key_Event_Data keys[KEY_INPUT_BUFFER_DSIZE];
};

inline Key_Event_Data
get_single_key(Key_Summary *summary, i32 index){
    Assert(index >= 0 && index < summary->count);
    Key_Event_Data key;
    key = summary->keys[index];
    return key;
}

struct Mouse_State{
	b32 out_of_window;
	b8 left_button, right_button;
	b8 left_button_pressed, right_button_pressed;
	b8 left_button_released, right_button_released;
	i16 wheel;
	i32 x, y;
};

struct Mouse_Summary{
    i32 mx, my;
    b32 l, r;
    b32 press_l, press_r;
    b32 release_l, release_r;
    b32 out_of_window;
    b32 wheel_used;
    i16 wheel_amount;
};

struct Input_Summary{
    Mouse_Summary mouse;
    Key_Summary keys;
    Key_Codes *codes;
};

// TODO(allen): This can go, and we can just use a String for it.
struct Clipboard_Contents{
	u8 *str;
	i32 size;
};

#define FileNameMax (1 << 9)

struct File_Slot{
    File_Slot *next, *prev;
    byte *data;
    i32 size, max;
    char *filename;
    i32 filename_len;
    u32 flags;
};

enum File_Exchange_Flag{
    FEx_Request = 0x1,
    FEx_Ready = 0x2,
    FEx_Not_Exist = 0x4,
    FEx_Save = 0x8,
    FEx_Save_Complete = 0x10
};

struct File_Exchange{
    File_Slot available, active, free_list;
    File_Slot *files;
    i32 num_active, max;
};

struct Exchange{
    Thread_Exchange thread;
    File_Exchange file;
};

#define App_Init_Sig(name)                                          \
    b32 name(System_Functions *system,                              \
             Render_Target *target,                                 \
             Application_Memory *memory,                            \
             Exchange *exchange,                                    \
             Key_Codes *loose_codes,                                \
             Clipboard_Contents clipboard,                          \
             String current_directory,                              \
             Custom_API api)

typedef App_Init_Sig(App_Init);

enum Application_Mouse_Cursor{
	APP_MOUSE_CURSOR_DEFAULT,
	APP_MOUSE_CURSOR_ARROW,
	APP_MOUSE_CURSOR_IBEAM,
	APP_MOUSE_CURSOR_LEFTRIGHT,
	APP_MOUSE_CURSOR_UPDOWN,
	// never below this
	APP_MOUSE_CURSOR_COUNT
};

struct Application_Step_Result{
	Application_Mouse_Cursor mouse_cursor_type;
    b32 redraw;
};

#define App_Step_Sig(name) Application_Step_Result          \
    name(System_Functions *system,                          \
         Key_Codes *codes,                                  \
         Key_Input_Data *input,                             \
         Mouse_State *mouse,                                \
         Render_Target *target,                             \
         Application_Memory *memory,                        \
         Exchange *exchange,                                \
         Clipboard_Contents clipboard,                      \
         b32 time_step, b32 first_step, b32 force_redraw)

typedef App_Step_Sig(App_Step);

#define App_Alloc_Sig(name) void *name(void *handle, i32 size)
typedef App_Alloc_Sig(App_Alloc);

#define App_Free_Sig(name) void name(void *handle, void *block)
typedef App_Free_Sig(App_Free);

struct App_Functions{
    App_Init *init;
    App_Step *step;

    App_Alloc *alloc;
    App_Free *free;
};

#define App_Get_Functions_Sig(name) App_Functions name()
typedef App_Get_Functions_Sig(App_Get_Functions);

#endif

// BOTTOM

