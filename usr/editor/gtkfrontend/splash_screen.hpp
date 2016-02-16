#ifndef SPLASH_SCREEN_H_INCLUDED
#define SPLASH_SCREEN_H_INCLUDED

namespace x3d
{
namespace usr
{

class SplashScreen
{
public:
        SplashScreen(EditorGtkFrontend* frontend);
        ~SplashScreen();

        bool show(bool visible);
public:
        class SplashScreenInt;
        class SplashScreenInt*  pimpl;
};

}// namespace usr

}// namespace x3d

#endif // SPLASH_SCREEN_H_INCLUDED
