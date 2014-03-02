/* basic_rt_shader.c: interfaces for basic rasterization shaders */
#include <renderer/shader.h>
#include <renderer/shader_lib.h>
#include "rtshader.h"
#include "basic_rt_shader_a.h"
#include "basic_rt_shader_so.h"


static void auto_fill_static ( struct shader_func *func )
{
	func->lib_form = SHADER_FUNC_STATIC;
	func->uni_addr[SHADER_UNI_MODELVIEW] = &uni_modelview;
}

void basic_rtshader_a_load ( void )
{
	auto_fill_static ( &lib_vs_to_modelview );
	shaderlib_load ( lib_vs_to_modelview.name, &lib_vs_to_modelview );
	auto_fill_static ( &lib_fs_pure_color );
	shaderlib_load ( lib_fs_pure_color.name, &lib_fs_pure_color );
}

void basic_rtshader_so_load ( void )
{
}
