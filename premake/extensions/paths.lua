local m = {}

local currentDir = os.getcwd();
function m.root()
    return path.getdirectory(path.getdirectory(currentDir));
end

function m.build(path1, ...)
    return path.join(m.root(), "build", "%{cfg.buildcfg:lower()}", path1, ...)
end

function m.deps(path1, ...)
    return path.join(m.root(), "deps", path1, ...)
end

function m.src()
    return path.join(m.root(), "src")
end

return m
