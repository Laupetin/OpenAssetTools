-- Platform functions
function ExecutableByOs(name)
    if os.host() == "windows" then
        return name .. ".exe"
    else
        return name
    end
end