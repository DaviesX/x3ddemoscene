#include <x3d/editor.h>


struct ax_render_conf {
};


void ax_render_conf_dispatch ( struct ax_render_conf *conf )
{
}

void ax_render_conf_update ( struct ax_render_conf *conf )
{
}

void ax_render_conf_load_state ( struct serializer *s, struct ax_render_conf *conf )
{
}

void ax_render_conf_save_state ( struct serializer *s, struct ax_render_conf *conf )
{
}

void ax_render_conf_free ( struct ax_render_conf *conf )
{
}

__dlexport struct ax_render_conf *create_ax_render_conf ( char *activate )
{
        return nullptr;
}

__dlexport void ax_render_conf_tab ( char *tab, struct ax_render_conf *conf )
{
}

__dlexport void ax_render_conf_value ( struct ax_render_conf *conf, char *target, ... )
{
}

__dlexport bool ax_render_conf_apply ( struct ax_render_conf *conf )
{
        return false;
}

__dlexport void ax_render_conf_cancel ( struct ax_render_conf *conf )
{
}

__dlexport void *ax_render_conf_checkout ( char *target, char *type, struct ax_render_conf *conf )
{
        return nullptr;
}

__dlexport void ax_render_conf_bind_signal ( char *signal, f_Generic func, void *info,
                                             struct ax_render_conf *conf )
{
}
