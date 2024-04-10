package("hashlibplus")
    set_homepage("https://github.com/ron4fun/HashLibPlus/")
    set_description("HashLibPlus is a recommended C++11 hashing library that provides a fluent interface for computing hashes and checksums of strings, files, streams, bytearrays and untyped data to mention but a few.")
    set_license("mpl-2.0")

    add_urls("https://github.com/WerWolv/HashLibPlus.git") -- Custom fork

    -- set language to c++20

    on_install(function (package)
        local configs = {}
        io.writefile("xmake.lua", [[
add_rules("mode.release", "mode.debug")

target("hashlibplus")
    set_languages("c++20")
    set_kind("$(kind)")
    add_files("Base/*.cpp", "Crypto/*.cpp", "Params/*.cpp", "Utils/*.cpp", "HashLibPlus.cpp")
    add_headerfiles("Base/*.h", {prefixdir = "Base"})
    add_headerfiles("Checksum/*.h", {prefixdir = "Checksum"})
    add_headerfiles("Crypto/*.h", {prefixdir = "Crypto"})
    add_headerfiles("Enum/*.h", {prefixdir = "Enum"})
    add_headerfiles("Hash32/*.h", {prefixdir = "Hash32"})
    add_headerfiles("Hash64/*.h", {prefixdir = "Hash64"})
    add_headerfiles("Hash128/*.h", {prefixdir = "Hash128"})
    add_headerfiles("Interfaces/IBlake2BConfigurations/*.h", {prefixdir = "Interfaces/IBlake2BConfigurations"})
    add_headerfiles("Interfaces/IBlake2SConfigurations/*.h", {prefixdir = "Interfaces/IBlake2SConfigurations"})
    add_headerfiles("Interfaces/*.h", {prefixdir = "Interfaces"})
    add_headerfiles("KDF/*.h", {prefixdir = "KDF"})
    add_headerfiles("MAC/*.h", {prefixdir = "MAC"})
    add_headerfiles("Nullable/*.h", {prefixdir = "Nullable"})
    add_headerfiles("NullDigest/*.h", {prefixdir = "NullDigest"})
    add_headerfiles("Params/*.h", {prefixdir = "Params"})
    add_headerfiles("Utils/*.h", {prefixdir = "Utils"})

    add_sysincludedirs("$(projectdir)")
        ]])
        if package:config("shared") then
            configs.kind = "shared"
        end
        -- for _, sourcefile in ipairs(os.files("*.cpp")) do
        --     io.replace(sourcefile, "#include <endian.h>", "", {plain = true})
        -- end
        import("package.tools.xmake").install(package, configs)
    end)

    on_test(function (package)
        assert(
            package:check_cxxsnippets(
                {test = "void test() { IHash HashInstance = HashFactory::Crypto::CreateMD5(); }"},
                {configs = {languages = "c++20"}, includes = {"Crypto/MD5.h", "Base/HashFactory.h", "Interfaces/IHash.h"}}
            )
        )
    end)
