#include <usr/usr_editor.hpp>


namespace x3d
{
namespace usr
{

class WorldDataActiveX::WorldDataInt
{
public:
        WorldDataInt();
        WorldDataInt(std::string data_set_name, std::string storage_path);
        ~WorldDataInt();

        bool                    has_data_set();
        
        bool                    m_has_data_changed = true;
        std::string             m_data_set_name = "<Empty Data Set>";
        std::string             m_storage_path = "./";
/* @todo (davis#2#): <WorldDataActiveX> */
        bool                    m_is_activated = false;
        World                   m_world;
        
        f_Notify_Error          f_notify_error = nullptr;
        void*                   f_notify_error_data = nullptr;
        std::list<std::string>  m_errors;
        
        f_Notify_Action         f_notify_action = nullptr;
        void*                   f_notify_action_data = nullptr;
        std::list<ActionInfo>   m_actions;
};

WorldDataActiveX::WorldDataInt::WorldDataInt()
{
}

WorldDataActiveX::WorldDataInt::WorldDataInt(std::string data_set_name, std::string storage_path) :
        m_data_set_name(data_set_name), m_storage_path(storage_path)
{
}

bool WorldDataActiveX::WorldDataInt::has_data_set()
{
        return (m_is_activated = m_world.get_render_aggregate()->get_instance_count() != 0);
}

WorldDataActiveX::WorldDataActiveX(string name) :
        EditorBackendActiveX(name, sizeof(WorldDataActiveX), EditActiveXWorldData)
{
        pimpl = new WorldDataActiveX::WorldDataInt;
}

WorldDataActiveX::WorldDataActiveX(std::string name, std::string data_set_name, std::string storage_path) :
        EditorBackendActiveX(name, sizeof(WorldDataActiveX), EditActiveXWorldData)
{
        pimpl = new WorldDataActiveX::WorldDataInt(data_set_name, storage_path);
}

WorldDataActiveX::~WorldDataActiveX()
{
}

void WorldDataActiveX::bind_callback(string signal, f_Generic callback, void *data)
{
        if (signal == "notify_error") {
                pimpl->f_notify_error = (f_Notify_Error) callback;
                pimpl->f_notify_error_data = data;
        } else if (signal == "notify_action") {
                pimpl->f_notify_action = (f_Notify_Action) callback;
                pimpl->f_notify_action_data = data;
        } else {
                log_mild_err("no such signal as: %s", signal.c_str());
        }
}

void WorldDataActiveX::new_data_set(std::string data_set_name, std::string storage_path)
{
        wait_for_update();
        pimpl->m_data_set_name = data_set_name;
        pimpl->m_storage_path = storage_path;
        // @todo #davis#2# <new_data_set> has to deal with world data
        unwait();
}

void WorldDataActiveX::open_data_set(std::string storage_path)
{
        // @todo #davis#2# <open_data_set> has to deal with world data
}

void WorldDataActiveX::close_data_set()
{
         wait_for_update();
         pimpl->m_data_set_name = "<Empty Data Set>";
         pimpl->m_storage_path  = "./";
        // @todo #davis#2# <open_data_set> has to deal with world data
         unwait();
}
        
std::string WorldDataActiveX::get_data_set_name()
{
        return pimpl->m_data_set_name;
}

std::string WorldDataActiveX::get_storage_path()
{
        return pimpl->m_storage_path;
}

void WorldDataActiveX::on_adding()
{
        // put itself to kernel environment
        KernelEnvironment* state = get_state_buffer();
        state->declare(c_WorldData, this);
}

void WorldDataActiveX::preupdate()
{
        pimpl->has_data_set();
}

void WorldDataActiveX::update()
{
        KernelEnvironment* state    = get_state_buffer();
//        RenderConfigActiveX* config = static_cast<RenderConfigActiveX*>(state->use(c_RenderConfig));
//        if (config) {
//                Renderer* renderer  = config->get_renderer();
//                pimpl->m_world.bind_render_processor(renderer);
//        }

        if (pimpl->m_is_activated) {
                pimpl->m_world.update();
        }
}

bool WorldDataActiveX::has_data_set()
{
        return pimpl->m_is_activated;
}

void WorldDataActiveX::dispatch()
{
        // prepare a vector of actions to prevent currency
        if (pimpl->f_notify_action) {
                wait_for_update();
                std::vector<ActionInfo> actions{pimpl->m_actions.begin(), pimpl->m_actions.end()};
                unwait();
                pimpl->f_notify_action(actions, *this, pimpl->f_notify_action_data);
        }
        if (pimpl->f_notify_error) {
                while (!pimpl->m_errors.empty()) {
                        pimpl->f_notify_error(pimpl->m_errors.back(), *this, pimpl->f_notify_error_data);
                        pimpl->m_errors.pop_back();
                }
        }
}

void WorldDataActiveX::load(struct serializer *s)
{
}

void WorldDataActiveX::save(struct serializer *s)
{
}

World& WorldDataActiveX::get_world()
{
        return pimpl->m_world;
}

}// namespace usr

}// namespace x3d
