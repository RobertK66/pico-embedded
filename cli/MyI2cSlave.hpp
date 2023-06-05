#ifdef __cplusplus
 #  define EXTERNC extern "C"
 #  define NOTHROW noexcept
 #else
 #  define EXTERNC
 #  define NOTHROW
 #endif

 /* Alias for your object in C that hides the implementation */
 typedef void* rk_myI2cSlave_t;

 /* Creates the object using the first constructor */
 EXTERNC rk_myI2cSlave_t rk_create_myI2cSlave() NOTHROW;

 /* Creates the object using the second constructor */
 EXTERNC rk_myI2cSlave_t rk_create_myI2cSlave_with_int(int a, int s) NOTHROW;

 /* Frees the object, using delete */
 EXTERNC void rk_free_myI2cSlave(rk_myI2cSlave_t obj) NOTHROW;


