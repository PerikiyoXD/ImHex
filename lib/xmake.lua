add_repositories("xmake-repo-local xmake-repo-local", {rootdir = os.projectdir()})

add_requires("capstone 5.0.1")
add_requires("fmt 10.2.1")
add_requires("nativefiledialog-extended v1.1.1")

-- https://github.com/ron4fun/HashLibPlus/
add_requires("hashlibplus")
-- https://github.com/Martinsos/edlib
-- add_requires("edlib v1.2.7")