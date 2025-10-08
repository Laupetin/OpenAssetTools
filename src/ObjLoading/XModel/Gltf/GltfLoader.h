#pragma once

#include "GltfInput.h"
#include "XModel/Gltf/JsonGltf.h"
#include "XModel/XModelFileLoader.h"

#include <memory>
#include <ostream>

namespace gltf
{
    class Loader : public XModelFileLoader
    {
    public:
        Loader() = default;
        ~Loader() override = default;
        Loader(const Loader& other) = default;
        Loader(Loader&& other) noexcept = default;
        Loader& operator=(const Loader& other) = default;
        Loader& operator=(Loader&& other) noexcept = default;

        /**
         * \brief Creates a loader capable of loading gltf-like files
         * \param input The gltf input
         * \param useBadRotationFormulas Old versions used bad formulas for converting into gltf space. Set to \c true to use them for loading to preserve
         * backwards compatibility.
         * \return
         */
        static std::unique_ptr<Loader> CreateLoader(const Input& input, bool useBadRotationFormulas);
    };
} // namespace gltf
