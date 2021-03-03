-- Functions for linking projects
Links = {}

function Links:create()

    list = {
        stack = {},
        dependencies = {}
    }

    function list:linkto(project)
        local projectName = project:name()
        for i, dependency in ipairs(self.stack) do
            if dependency == projectName then
                local errorMessage = "Circular dependency detected:\n"
                errorMessage = errorMessage .. projectName
                for j = i + 1, #self.stack do
                    errorMessage = errorMessage .. " -> " .. self.stack[j]
                end
                errorMessage = errorMessage .. " -> " .. projectName
                premake.error(errorMessage)
                print(errorMessage)
                os.exit(1)
            end
        end

        table.insert(self.stack, projectName)
        project:link(self)
        table.remove(self.stack)
    end

    function list:add(name)
        for i = 1, #self.dependencies do
            if self.dependencies[i] == name then
                table.remove(self.dependencies, i)
                i = i - 1
            end
        end
        table.insert(self.dependencies, name)
    end

    function list:linkall()
        for i, dependency in ipairs(self.dependencies) do
            links(dependency)
        end
    end

    return list
end