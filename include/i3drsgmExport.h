// i3drsgmExport.h
#pragma once

// Define EXPORTED for any platform
#ifdef _WIN32
# ifdef I3DRSGM_EXPORT
#   define I3DRSGM_EXPORTS  __declspec( dllexport )
# else
#   define I3DRSGM_EXPORTS  __declspec( dllimport )
# endif
#else
# define I3DRSGM_EXPORTS
#endif