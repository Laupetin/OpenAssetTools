#pragma once

#include "GltfInput.h"
#include "XModel/Gltf/JsonGltf.h"
#include "XModel/XModelLoader.h"

#include <memory>
#include <ostream>

namespace gltf
{
    class Loader : public XModelLoader
    {
    public:
        Loader() = default;
        ~Loader() override = default;
        Loader(const Loader& other) = default;
        Loader(Loader&& other) noexcept = default;
        Loader& operator=(const Loader& other) = default;
        Loader& operator=(Loader&& other) noexcept = default;

        static std::unique_ptr<Loader> CreateLoader(const Input* input);
    };
} // namespace gltf
