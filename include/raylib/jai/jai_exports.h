#include <stdint.h>

typedef uint64_t     u64;
typedef uint32_t     u32;
typedef uint16_t     u16;
typedef uint8_t      u8;
typedef int64_t      s64;
typedef int32_t      s32;
typedef int16_t      s16;
typedef int8_t       s8;
typedef double       float64;
typedef float        float32;

struct Type_Info_Struct;

#ifdef __cplusplus
extern "C" {
    #define null (0)
#else
    typedef s8          bool;
    const bool false = 0;
    const bool true = 1;
#endif

struct JAI_Type_Info;
struct JAI_string;
struct JAI_Array_View_64;
struct JAI_Type_Info_Struct;
struct JAI_Allocator;
struct JAI_Source_Code_Location;
struct JAI_Log_Section;
struct JAI_Log_Info;
struct JAI_Overflow_Page;
struct JAI_Temporary_Storage;
struct JAI_Stack_Trace_Procedure_Info;
struct JAI_Stack_Trace_Node;
struct JAI_Context_Base;
struct JAI_Any;
struct JAI_Formatter;
struct JAI_FormatInt;
struct JAI_FormatFloat;
struct JAI_FormatStruct;
struct JAI_FormatArray;
struct JAI_Buffer;
struct JAI_String_Builder;
struct JAI_Print_Style;
struct JAI_Random_State;
struct JAI_Context;

enum JAI_Type_Info_Tag {
    TIT_INTEGER = 0,
    TIT_FLOAT = 1,
    TIT_BOOL = 2,
    TIT_STRING = 3,
    TIT_POINTER = 4,
    TIT_PROCEDURE = 5,
    TIT_VOID = 6,
    TIT_STRUCT = 7,
    TIT_ARRAY = 8,
    TIT_OVERLOAD_SET = 9,
    TIT_ANY = 10,
    TIT_ENUM = 11,
    TIT_POLYMORPHIC_VARIABLE = 12,
    TIT_TYPE = 13,
    TIT_CODE = 14,
    TIT_VARIANT = 18,
};

typedef struct JAI_Type_Info {
    u32 type;
    s64 runtime_size;
} JAI_Type_Info;

typedef struct JAI_string {
    s64 count;
    u8 *data;
} JAI_string;

typedef struct JAI_Array_View_64 {
    s64 count;
    u8 *data;
} JAI_Array_View_64;

enum JAI_Struct_Status_Flags {
    SSF_INCOMPLETE = 1,
    SSF_LOCAL = 4,
};

enum JAI_Struct_Nontextual_Flags {
    SNF_NOT_INSTANTIABLE = 4,
    SNF_ALL_MEMBERS_UNINITIALIZED = 64,
    SNF_POLYMORPHIC = 256,
};

enum JAI_Struct_Textual_Flags {
    STF_FOREIGN = 1,
    STF_UNION = 2,
    STF_NO_PADDING = 4,
    STF_TYPE_INFO_NONE = 8,
    STF_TYPE_INFO_NO_SIZE_COMPLAINT = 16,
    STF_TYPE_INFO_PROCEDURES_ARE_VOID_POINTERS = 32,
};

typedef struct JAI_Type_Info_Struct {
    struct JAI_Type_Info info;
    JAI_string name;
    JAI_Array_View_64 specified_parameters;
    JAI_Array_View_64 members;
    u32 status_flags;
    u32 nontextual_flags;
    u32 textual_flags;
    struct JAI_Type_Info_Struct *polymorph_source_struct;
    void *initializer;
    JAI_Array_View_64 constant_storage;
    JAI_Array_View_64 notes;
} JAI_Type_Info_Struct;

enum JAI_Allocator_Mode {
    AM_ALLOCATE = 0,
    AM_RESIZE = 1,
    AM_FREE = 2,
    AM_STARTUP = 3,
    AM_SHUTDOWN = 4,
    AM_THREAD_START = 5,
    AM_THREAD_STOP = 6,
    AM_CREATE_HEAP = 7,
    AM_DESTROY_HEAP = 8,
    AM_IS_THIS_YOURS = 9,
    AM_CAPS = 10,
};

typedef struct JAI_Allocator {
    void *proc;
    void *data;
} JAI_Allocator;

typedef struct JAI_Source_Code_Location {
    JAI_string fully_pathed_filename;
    s64 line_number;
    s64 character_number;
} JAI_Source_Code_Location;

enum JAI_Log_Flags {
    LF_NONE = 0,
    LF_ERROR = 1,
    LF_WARNING = 2,
    LF_CONTENT = 4,
    LF_TO_FILE_ONLY = 8,
    LF_VERBOSE_ONLY = 16,
    LF_VERY_VERBOSE_ONLY = 32,
    LF_TOPIC_ONLY = 64,
};

typedef struct JAI_Log_Section {
    JAI_string name;
} JAI_Log_Section;

typedef struct JAI_Log_Info {
    u64 source_identifier;
    struct JAI_Source_Code_Location location;
    u32 common_flags;
    u32 user_flags;
    struct JAI_Log_Section *section;
} JAI_Log_Info;

enum JAI_Log_Level {
    LL_NORMAL = 0,
    LL_VERBOSE = 1,
    LL_VERY_VERBOSE = 2,
};

typedef struct JAI_Overflow_Page {
    struct JAI_Overflow_Page *next;
    struct JAI_Allocator allocator;
    s64 size;
} JAI_Overflow_Page;

typedef struct JAI_Temporary_Storage {
    u8 *data;
    s64 size;
    s64 current_page_bytes_occupied;
    s64 total_bytes_occupied;
    s64 high_water_mark;
    struct JAI_Source_Code_Location last_set_mark_location;
    struct JAI_Allocator overflow_allocator;
    struct JAI_Overflow_Page *overflow_pages;
    u8 *original_data;
    s64 original_size;
} JAI_Temporary_Storage;

typedef struct JAI_Stack_Trace_Procedure_Info {
    JAI_string name;
    struct JAI_Source_Code_Location location;
    void *procedure_address;
} JAI_Stack_Trace_Procedure_Info;

typedef struct JAI_Stack_Trace_Node {
    struct JAI_Stack_Trace_Node *next;
    struct JAI_Stack_Trace_Procedure_Info *info;
    u64 hash;
    u32 call_depth;
    u32 line_number;
} JAI_Stack_Trace_Node;

typedef struct JAI_Context_Base {
    struct JAI_Type_Info_Struct *context_info;
    u32 thread_index;
    struct JAI_Allocator allocator;
    void *logger;
    void *logger_data;
    u64 log_source_identifier;
    u8 log_level;
    struct JAI_Temporary_Storage *temporary_storage;
    struct JAI_Stack_Trace_Node *stack_trace;
    void *assertion_failed;
    bool handling_assertion_failure;
    void *program_print_plugin;
} JAI_Context_Base;

typedef struct JAI_Any {
    struct JAI_Type_Info *type;
    void *value_pointer;
} JAI_Any;

typedef struct JAI_Formatter {
    JAI_Any value;
} JAI_Formatter;

typedef struct JAI_FormatInt {
    struct JAI_Formatter formatter;
    s64 base;
    s64 minimum_digits;
    u8 padding;
    u16 digits_per_comma;
    JAI_string comma_string;
} JAI_FormatInt;

enum JAI_Zero_Removal {
    ZR_YES = 0,
    ZR_NO = 1,
    ZR_ONE_ZERO_AFTER_DECIMAL = 2,
};

enum JAI_Mode {
    M_DECIMAL = 0,
    M_SCIENTIFIC = 1,
    M_SHORTEST = 2,
};

enum JAI_Positive_Number_Prefix {
    PNP_NONE = 0,
    PNP_PLUS = 1,
    PNP_SPACE = 2,
};

typedef struct JAI_FormatFloat {
    struct JAI_Formatter formatter;
    s64 width;
    s64 trailing_width;
    s64 zero_removal;
    s64 mode;
    bool thousand_separators;
    u8 thousand_separator_character;
    u8 decimal_separator_character;
    u8 exponent_character;
    s64 positive_number_prefix;
} JAI_FormatFloat;

typedef struct JAI_FormatStruct {
    struct JAI_Formatter formatter;
    bool draw_type_name;
    s64 use_long_form_if_more_than_this_many_members;
    JAI_string separator_between_name_and_value;
    JAI_string short_form_separator_between_fields;
    JAI_string long_form_separator_between_fields;
    JAI_string begin_string;
    JAI_string end_string;
    s32 indentation_width;
    bool use_newlines_if_long_form;
} JAI_FormatStruct;

typedef struct JAI_FormatArray {
    struct JAI_Formatter formatter;
    JAI_string separator;
    JAI_string begin_string;
    JAI_string end_string;
    JAI_string printing_stopped_early_string;
    bool draw_separator_after_last_element;
    s64 stop_printing_after_this_many_elements;
} JAI_FormatArray;

typedef struct JAI_Buffer {
    s64 count;
    s64 allocated;
    s64 ensured_count;
    struct JAI_Buffer *next;
} JAI_Buffer;

typedef struct JAI_String_Builder {
    bool initialized;
    bool failed;
    s64 subsequent_buffer_size;
    struct JAI_Allocator allocator;
    struct JAI_Buffer *current_buffer;
    u8 initial_bytes[4064];
} JAI_String_Builder;

typedef struct JAI_Print_Style {
    struct JAI_FormatInt default_format_int;
    struct JAI_FormatFloat default_format_float;
    struct JAI_FormatStruct default_format_struct;
    struct JAI_FormatArray default_format_array;
    struct JAI_FormatInt default_format_absolute_pointer;
    void *struct_printer;
    void *struct_printer_data;
    s32 indentation_depth;
    bool log_runtime_errors;
} JAI_Print_Style;

typedef struct JAI_Random_State {
    u64 low;
    u64 high;
} JAI_Random_State;

typedef struct JAI_Context {
    struct JAI_Context_Base base;
    struct JAI_Print_Style print_style;
    struct JAI_Random_State random_state;
} JAI_Context;


s64 jaiMulti(s64 a, s64 b);

s64 jaiAdd(s64 a, s64 b);

void __jai_runtime_fini(void *_context);

struct JAI_Context * __jai_runtime_init(s32 argc, u8 **argv);


#ifdef __cplusplus
} // extern "C"
#endif
