#include <usr/usr_editor.hpp>
#include <system/thread.h>


namespace x3d
{
namespace usr
{

class PowersaveSwitchActiveX::PowersaveSwitchInt
{
public:
        bool    m_is_on_powersave = true;
};


PowersaveSwitchActiveX::PowersaveSwitchActiveX(string name) :
        EditorBackendActiveX(name, sizeof(PowersaveSwitchActiveX), EditActiveXPowersaveSwitch)
{
        pimpl = new PowersaveSwitchActiveX::PowersaveSwitchInt;
}

PowersaveSwitchActiveX::~PowersaveSwitchActiveX()
{
        delete pimpl;
}

void PowersaveSwitchActiveX::on_adding()
{
        // put itself to kernel environment
        KernelEnvironment* state = get_state_buffer();
        state->declare(c_PowersaveSwitch, this);
}

void PowersaveSwitchActiveX::update()
{
        if (pimpl->m_is_on_powersave) {
                thread_task_idle(10);
        }
}

void PowersaveSwitchActiveX::dispatch()
{
}

void PowersaveSwitchActiveX::load(struct serializer *s)
{
}

void PowersaveSwitchActiveX::save(struct serializer *s)
{
}

void PowersaveSwitchActiveX::switch_on_performance()
{
        pimpl->m_is_on_powersave = false;
}

void PowersaveSwitchActiveX::switch_on_powersave()
{
        pimpl->m_is_on_powersave = true;
}

}// namespace usr

}// namespace x3d
