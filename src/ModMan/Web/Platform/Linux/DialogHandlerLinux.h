#pragma once

#ifdef __linux__

#include "Web/Platform/Platform.h"

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace PLATFORM_NAMESPACE_LINUX
{
    class FileDialogFilter
    {
    public:
        FileDialogFilter(std::string name, std::string filter);

        std::string m_name;
        std::string m_filter;
    };

    class DialogWithCallback
    {
    public:
        DialogWithCallback();

        void SetCallback(std::function<void(std::optional<std::string> result)> callback);

    protected:
        std::function<void(std::optional<std::string> result)> m_callback;
    };

    class FileDialog : public DialogWithCallback
    {
    public:
        FileDialog();

        void AddFilter(std::string name, std::string filter);

    protected:
        std::vector<FileDialogFilter> m_filters;
    };

    class OpenFileDialog : public FileDialog
    {
    public:
        OpenFileDialog();

        void OpenAsync() const;
    };

    class SaveFileDialog : public FileDialog
    {
    public:
        SaveFileDialog();

        void OpenAsync() const;
    };

    class FolderSelectDialog : public DialogWithCallback
    {
    public:
        FolderSelectDialog();

        void OpenAsync() const;
    };
} // namespace PLATFORM_NAMESPACE_LINUX

#endif
