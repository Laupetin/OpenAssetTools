#pragma once

#include "Game/IGame.h"
#include "LinkerArgs.h"
#include "SearchPath/ISearchPath.h"

#include <memory>

class ILinkerSearchPathBuilder
{
public:
    ILinkerSearchPathBuilder() = default;
    virtual ~ILinkerSearchPathBuilder() = default;
    ILinkerSearchPathBuilder(const ILinkerSearchPathBuilder& other) = default;
    ILinkerSearchPathBuilder(ILinkerSearchPathBuilder&& other) noexcept = default;
    ILinkerSearchPathBuilder& operator=(const ILinkerSearchPathBuilder& other) = default;
    ILinkerSearchPathBuilder& operator=(ILinkerSearchPathBuilder&& other) noexcept = default;

    [[nodiscard]] virtual std::unique_ptr<ISearchPath> BuildIndependentSearchPaths() const = 0;
    [[nodiscard]] virtual std::unique_ptr<ISearchPath> BuildSearchPathsSpecificToProject(const std::string& projectName) const = 0;
    [[nodiscard]] virtual std::unique_ptr<ISearchPath> BuildSearchPathsSpecificToProjectAndGame(const std::string& projectName, GameId game) const = 0;
};

class ILinkerPaths
{
public:
    ILinkerPaths() = default;
    virtual ~ILinkerPaths() = default;
    ILinkerPaths(const ILinkerPaths& other) = default;
    ILinkerPaths(ILinkerPaths&& other) noexcept = default;
    ILinkerPaths& operator=(const ILinkerPaths& other) = default;
    ILinkerPaths& operator=(ILinkerPaths&& other) noexcept = default;

    /**
     * \brief Creates linker search paths based on templates from user specified args.
     * \param args The user specified args.
     * \return Linker search paths based on user specified templates.
     */
    static std::unique_ptr<ILinkerPaths> FromArgs(const LinkerArgs& args);

    /**
     * \brief Grants access to the builder for asset search paths.
     * \return A builder instance for building asset search paths.
     */
    [[nodiscard]] virtual const ILinkerSearchPathBuilder& AssetSearchPaths() const = 0;

    /**
     * \brief Grants access to the builder for gdt search paths.
     * \return A builder instance for building gdt search paths.
     */
    [[nodiscard]] virtual const ILinkerSearchPathBuilder& GdtSearchPaths() const = 0;

    /**
     * \brief Grants access to the builder for source search paths.
     * \return A builder instance for building source search paths.
     */
    [[nodiscard]] virtual const ILinkerSearchPathBuilder& SourceSearchPaths() const = 0;

    /**
     * \brief Builds the cache path based on the specified information.
     * \param projectName The name of the project to resolve the path input for.
     * \param game The game to resolve the path input for.
     * \return A cache path based on the input and preconfigured template.
     */
    [[nodiscard]] virtual std::string BuildCacheFolderPath(const std::string& projectName, GameId game) const = 0;

    /**
     * \brief Builds the output path based on the specified information.
     * \param projectName The name of the project to resolve the path input for.
     * \param game The game to resolve the path input for.
     * \return An output path based on the input and preconfigured template.
     */
    [[nodiscard]] virtual std::string BuildOutputFolderPath(const std::string& projectName, GameId game) const = 0;
};
