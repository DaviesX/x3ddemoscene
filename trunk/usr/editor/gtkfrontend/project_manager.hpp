#ifndef PROJECT_MANAGER_HPP_INCLUDED
#define PROJECT_MANAGER_HPP_INCLUDED


namespace x3d
{
namespace usr
{

class ProjectManager
{
public:
        ProjectManager(EditorGtkFrontend* frontend);
        ~ProjectManager();

        bool show(bool visible);
private:
        class ProjectManagerInt;
        class ProjectManagerInt*        pimpl;
};

}// namespace usr

}// namespace x3d


#endif // PROJECT_MANAGER_HPP_INCLUDED
