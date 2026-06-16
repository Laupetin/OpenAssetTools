#include "DialogBinds.h"

#include "Web/Platform/DialogHandler.h"
#include "Web/UiCommunication.h"

#include "Json/JsonExtension.h"

namespace
{
    class FileDialogFilterDto
    {
    public:
        std::string name;
        std::string filter;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(FileDialogFilterDto, name, filter);

    class OpenFileDialogDto
    {
    public:
        std::optional<std::vector<FileDialogFilterDto>> filters;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(OpenFileDialogDto, filters);

    class SaveFileDialogDto
    {
    public:
        std::optional<std::vector<FileDialogFilterDto>> filters;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(SaveFileDialogDto, filters);

    void ReplyWithDialogResult(webwindowed::detail::window_base& calling_window,
                               const std::string& id,
                               const ui::DialogCallbackResultType resultType,
                               const std::optional<std::string>& result)
    {
        if (resultType == ui::DialogCallbackResultType::FAILED)
            ui::PromiseReject(calling_window, id, result);
        else
            ui::PromiseResolve(calling_window, id, result);
    }

    void OpenFileDialogBind(webwindowed::detail::window_base& calling_window, const std::string& id, const std::optional<OpenFileDialogDto>& dto)
    {
        ui::OpenFileDialog dialog;
        dialog.SetCallback(
            [&calling_window, id](const ui::DialogCallbackResultType resultType, const std::optional<std::string>& result)
            {
                ReplyWithDialogResult(calling_window, id, resultType, result);
            });

        if (dto && dto->filters)
        {
            for (auto& filter : *dto->filters)
            {
                dialog.AddFilter(filter.name, filter.filter);
            }
        }

        dialog.OpenAsync();
    }

    void SaveFileDialogBind(webwindowed::detail::window_base& calling_window, const std::string& id, const std::optional<SaveFileDialogDto>& dto)
    {
        ui::SaveFileDialog dialog;
        dialog.SetCallback(
            [&calling_window, id](const ui::DialogCallbackResultType resultType, const std::optional<std::string>& result)
            {
                ReplyWithDialogResult(calling_window, id, resultType, result);
            });

        if (dto && dto->filters)
        {
            for (auto& filter : *dto->filters)
            {
                dialog.AddFilter(filter.name, filter.filter);
            }
        }

        dialog.OpenAsync();
    }

    void FolderSelectDialogBind(webwindowed::detail::window_base& calling_window, const std::string& id)
    {
        ui::FolderSelectDialog dialog;
        dialog.SetCallback(
            [&calling_window, id](const ui::DialogCallbackResultType resultType, const std::optional<std::string>& result)
            {
                ReplyWithDialogResult(calling_window, id, resultType, result);
            });

        dialog.OpenAsync();
    }
} // namespace

namespace ui
{
    void RegisterDialogHandlerBinds(webwindowed::commands_builder& commands)
    {
        BindAsync<std::optional<OpenFileDialogDto>>(
            commands,
            "openFileDialog",
            [](const std::string& id, webwindowed::detail::window_base& calling_window, const std::optional<OpenFileDialogDto>& dto)
            {
                OpenFileDialogBind(calling_window, id, dto);
            });

        BindAsync<std::optional<SaveFileDialogDto>>(
            commands,
            "saveFileDialog",
            [](const std::string& id, webwindowed::detail::window_base& calling_window, const std::optional<SaveFileDialogDto>& dto)
            {
                SaveFileDialogBind(calling_window, id, dto);
            });

        BindAsync(commands,
                  "folderSelectDialog",
                  [](const std::string& id, webwindowed::detail::window_base& calling_window)
                  {
                      FolderSelectDialogBind(calling_window, id);
                  });
    }
} // namespace ui
