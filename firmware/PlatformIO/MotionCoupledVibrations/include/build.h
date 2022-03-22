// While developing, it is useful to have some debug information. In contrast,
// the release build should be optimized for speed and shouldn't have any debug
// information. In the "platformio.ini" file you can specify the build type.
#if SENSINT_BUILD_MODE == 0
#define SENSINT_DEVELOPMENT
#else
#define SENSINT_RELEASE
#undef SENSINT_DEBUG
#endif
