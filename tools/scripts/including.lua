-- Functions for including projects
Includes = {}

function Includes:create()

    list = {
        handles = {}
    }

    function list:handle(name)
        if self.handles[name] == nil then
            self.handles[name] = true
            return true
        end

        return false
    end

    return list
end