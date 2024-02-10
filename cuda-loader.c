#include <stdio.h>

#include <cuda.h>

#ifdef _WIN32
    #include <windows.h>
    #define CUDA_DRIVER_LIBRARY "nvcuda.dll"
    typedef HMODULE lib_handle;
    #define load_library(name) LoadLibrary(name)
    #define load_symbol(lib, symbol) GetProcAddress(lib, symbol)
    #define close_library(lib) FreeLibrary(lib)
#else
    #include <dlfcn.h>
    #define CUDA_DRIVER_LIBRARY "libcuda.so"
    #define CUDA_DRIVER_LIBRARY_ALT "libcuda.so.1"
    typedef void* lib_handle;
    #define load_library(name) dlopen(name, RTLD_NOW)
    #define load_symbol(lib, symbol) dlsym(lib, symbol)
    #define close_library(lib) dlclose(lib)
#endif


typedef CUresult (*cuDeviceGet_pt)(CUdevice *device, int ordinal);
typedef CUresult (*cuDeviceGetAttribute_pt)(int *pi, CUdevice_attribute attrib, CUdevice dev);
typedef CUresult (*cuGetErrorString_pt)(CUresult error, const char **pStr);
typedef CUresult (*cuMemGetAllocationGranularity_pt)(size_t *granularity, const CUmemAllocationProp *prop, CUmemAllocationGranularity_flags option);
typedef CUresult (*cuMemCreate_pt)(CUmemGenericAllocationHandle *handle, size_t size, const CUmemAllocationProp *prop, unsigned long long flags);
typedef CUresult (*cuMemAddressReserve_pt)(CUdeviceptr *ptr, size_t size, size_t alignment, CUdeviceptr addr, unsigned long long flags);
typedef CUresult (*cuMemMap_pt)(CUdeviceptr ptr, size_t size, size_t offset, CUmemGenericAllocationHandle handle, unsigned long long flags);
typedef CUresult (*cuMemRelease_pt)(CUmemGenericAllocationHandle handle);
typedef CUresult (*cuMemSetAccess_pt)(CUdeviceptr ptr, size_t size, const CUmemAccessDesc *desc, size_t count);


cuDeviceGet_pt _cuDeviceGet = NULL;
cuDeviceGetAttribute_pt _cuDeviceGetAttribute = NULL;
cuGetErrorString_pt _cuGetErrorString = NULL;
cuMemGetAllocationGranularity_pt _cuMemGetAllocationGranularity = NULL;
cuMemCreate_pt _cuMemCreate = NULL;
cuMemAddressReserve_pt _cuMemAddressReserve = NULL;
cuMemMap_pt _cuMemMap = NULL;
cuMemRelease_pt _cuMemRelease = NULL;
cuMemSetAccess_pt _cuMemSetAccess = NULL;


int load_libcuda(void) {

    static lib_handle libcuda = NULL;

    if (libcuda == (lib_handle)1)
        return 0;

    if (libcuda != NULL)
        return 1;

    libcuda = load_library(CUDA_DRIVER_LIBRARY);

#ifdef CUDA_DRIVER_LIBRARY_ALT
    if (libcuda == NULL) {
        libcuda = load_library(CUDA_DRIVER_LIBRARY_ALT);
    }
#endif

    if (libcuda != NULL) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
        _cuDeviceGet = (cuDeviceGet_pt)load_symbol(libcuda, "cuDeviceGet");
        _cuDeviceGetAttribute = (cuDeviceGetAttribute_pt)load_symbol(libcuda, "cuDeviceGetAttribute");
        _cuGetErrorString = (cuGetErrorString_pt)load_symbol(libcuda, "cuGetErrorString");
        _cuMemGetAllocationGranularity = (cuMemGetAllocationGranularity_pt)load_symbol(libcuda, "cuMemGetAllocationGranularity");
        _cuMemCreate = (cuMemCreate_pt)load_symbol(libcuda, "cuMemCreate");
        _cuMemAddressReserve = (cuMemAddressReserve_pt)load_symbol(libcuda, "cuMemAddressReserve");
        _cuMemMap = (cuMemMap_pt)load_symbol(libcuda, "cuMemMap");
        _cuMemRelease = (cuMemRelease_pt)load_symbol(libcuda, "cuMemRelease");
        _cuMemSetAccess = (cuMemSetAccess_pt)load_symbol(libcuda, "cuMemSetAccess");
#pragma GCC diagnostic pop

        return 1;
    }

    fprintf(stderr, "error: failed to load the CUDA driver: %s\n", dlerror());

    libcuda = (lib_handle)1;   // tried and failed
    return 0;
}


CUresult CUDAAPI cuDeviceGet(CUdevice *device, int ordinal) {
    if (_cuDeviceGet == NULL && !load_libcuda())
        return CUDA_ERROR_SHARED_OBJECT_INIT_FAILED;
    if (_cuDeviceGet == NULL)
        return CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND;
    return _cuDeviceGet(device, ordinal);
}

CUresult CUDAAPI cuDeviceGetAttribute(int *pi, CUdevice_attribute attrib, CUdevice dev) {
    if (_cuDeviceGetAttribute == NULL && !load_libcuda())
        return CUDA_ERROR_SHARED_OBJECT_INIT_FAILED;
    if (_cuDeviceGetAttribute == NULL)
        return CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND;
    return _cuDeviceGetAttribute(pi, attrib, dev);
}

CUresult CUDAAPI cuGetErrorString(CUresult error, const char **pStr) {
    if (_cuGetErrorString == NULL && !load_libcuda())
        return CUDA_ERROR_SHARED_OBJECT_INIT_FAILED;
    if (_cuGetErrorString == NULL)
        return CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND;
    return _cuGetErrorString(error, pStr);
}

CUresult CUDAAPI cuMemGetAllocationGranularity(size_t *granularity, const CUmemAllocationProp *prop, CUmemAllocationGranularity_flags option) {
    if (_cuMemGetAllocationGranularity == NULL && !load_libcuda())
        return CUDA_ERROR_SHARED_OBJECT_INIT_FAILED;
    if (_cuMemGetAllocationGranularity == NULL)
        return CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND;
    return _cuMemGetAllocationGranularity(granularity, prop, option);
}

CUresult CUDAAPI cuMemCreate(CUmemGenericAllocationHandle *handle, size_t size, const CUmemAllocationProp *prop, unsigned long long flags) {
    if (_cuMemCreate == NULL && !load_libcuda())
        return CUDA_ERROR_SHARED_OBJECT_INIT_FAILED;
    if (_cuMemCreate == NULL)
        return CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND;
    return _cuMemCreate(handle, size, prop, flags);
}

CUresult CUDAAPI cuMemAddressReserve(CUdeviceptr *ptr, size_t size, size_t alignment, CUdeviceptr addr, unsigned long long flags) {
    if (_cuMemAddressReserve == NULL && !load_libcuda())
        return CUDA_ERROR_SHARED_OBJECT_INIT_FAILED;
    if (_cuMemAddressReserve == NULL)
        return CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND;
    return _cuMemAddressReserve(ptr, size, alignment, addr, flags);
}

CUresult CUDAAPI cuMemMap(CUdeviceptr ptr, size_t size, size_t offset, CUmemGenericAllocationHandle handle, unsigned long long flags) {
    if (_cuMemMap == NULL && !load_libcuda())
        return CUDA_ERROR_SHARED_OBJECT_INIT_FAILED;
    if (_cuMemMap == NULL)
        return CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND;
    return _cuMemMap(ptr, size, offset, handle, flags);
}

CUresult CUDAAPI cuMemRelease(CUmemGenericAllocationHandle handle) {
    if (_cuMemRelease == NULL && !load_libcuda())
        return CUDA_ERROR_SHARED_OBJECT_INIT_FAILED;
    if (_cuMemRelease == NULL)
        return CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND;
    return _cuMemRelease(handle);
}

CUresult CUDAAPI cuMemSetAccess(CUdeviceptr ptr, size_t size, const CUmemAccessDesc *desc, size_t count) {
    if (_cuMemSetAccess == NULL && !load_libcuda())
        return CUDA_ERROR_SHARED_OBJECT_INIT_FAILED;
    if (_cuMemSetAccess == NULL)
        return CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND;
    return _cuMemSetAccess(ptr, size, desc, count);
}
