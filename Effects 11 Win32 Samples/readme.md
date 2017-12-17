https://code.msdn.microsoft.com/Effects-11-Win32-Samples-cce82a4d

需要把effct的D3DXGlobal.h文件的static改成inline 
```                                                                                                                                                                                                                          /*static*/ inline void* __cdecl operator new(_In_ size_t s, _In_ CDataBlockStore &pAllocator)
{
#ifdef _M_X64
    assert( s <= 0xffffffff );
#endif
    return pAllocator.Allocate( (uint32_t)s );
}

/*static*/ inline void __cdecl operator delete(_In_opt_ void* p, _In_ CDataBlockStore &pAllocator)
{
    UNREFERENCED_PARAMETER(p);
    UNREFERENCED_PARAMETER(pAllocator);
}
```                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              