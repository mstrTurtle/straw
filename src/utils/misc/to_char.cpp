template<typename _Ty, size_t _Size>
constexpr void to_char(char (&buffer)[_Size], const _Ty& i) noexcept
{
    const len = sizeof(_Ty);
    static_assert(_Size>len);
    memcpy(buffer,(char*)&i,size);
    buffer[len] = '\0';
}