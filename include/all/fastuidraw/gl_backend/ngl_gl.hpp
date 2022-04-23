#pragma once

#include <GL/glcorearb.h>


namespace fastuidraw {
namespace gl_binding {
void* get_proc(const char *name);
void load_all_functions(bool emit_load_warning);

#define FASTUIDRAWglfunctionExists(name) fastuidraw::gl_binding::exists_function_##name()

#define FASTUIDRAWglfunctionPointer(name) fastuidraw::gl_binding::get_function_ptr_##name()



} //fastuidraw

} //gl_binding