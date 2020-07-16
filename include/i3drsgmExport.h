#ifndef I3DRSGM_EXPORT_H
#define I3DRSGM_EXPORT_H
    #ifdef I3DRSGM_EXPORT
        #define I3DRSGM_EXPORTS __declspec(dllexport)
    #else
        #define I3DRSGM_EXPORTS __declspec(dllimport)
    #endif
#endif //I3DRSGM_EXPORT_H