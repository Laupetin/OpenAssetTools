#include "Web/Platform/DialogHandler.h"

#ifdef _WIN32

#include "PlatformUtilsWindows.h"

#include <shobjidl.h>
#include <thread>
#include <windows.h>

using namespace PLATFORM_NAMESPACE_WINDOWS;

namespace
{
    bool SetFilters(IFileDialog* pFileOpen, const std::vector<ui::FileDialogFilter>& filters)
    {
        if (filters.empty())
            return true;

        const auto filterCount = filters.size();
        std::vector<COMDLG_FILTERSPEC> filterSpecs;
        filterSpecs.reserve(filterCount + 1);

        std::vector<std::wstring> filterStrings;
        filterStrings.reserve(filterCount * 2);

        for (auto i = 0u; i < filterCount; i++)
        {
            const auto& filter = filters[i];
            COMDLG_FILTERSPEC filterSpec;

            const auto& wName = filterStrings.emplace_back(StringToWideString(filter.m_name));
            const auto& wSpec = filterStrings.emplace_back(StringToWideString(filter.m_filter));

            filterSpec.pszName = wName.c_str();
            filterSpec.pszSpec = wSpec.c_str();

            filterSpecs.emplace_back(filterSpec);
        }

        COMDLG_FILTERSPEC wildCardSpec;
        wildCardSpec.pszName = L"All files";
        wildCardSpec.pszSpec = L"*.*";
        filterSpecs.emplace_back(wildCardSpec);

        const auto result = pFileOpen->SetFileTypes(static_cast<UINT>(filterCount + 1), filterSpecs.data());

        return SUCCEEDED(result);
    }

    ui::DialogCallbackResultType ShowFileDialog(IFileDialog* pFileDialog, std::optional<std::string>& result)
    {
        auto resultType = ui::DialogCallbackResultType::FAILED;

        auto hr = pFileDialog->Show(nullptr);
        if (SUCCEEDED(hr))
        {
            IShellItem* pItem;
            hr = pFileDialog->GetResult(&pItem);
            if (SUCCEEDED(hr))
            {
                PWSTR pszFilePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                // Display the file name to the user.
                if (SUCCEEDED(hr))
                {
                    result = WideStringToString(pszFilePath);
                    CoTaskMemFree(pszFilePath);

                    resultType = ui::DialogCallbackResultType::SUCCESSFUL;
                }
                pItem->Release();
            }
        }
        else if (HRESULT_FROM_WIN32(ERROR_CANCELLED) == hr)
        {
            resultType = ui::DialogCallbackResultType::CANCELLED;
        }

        return resultType;
    }
} // namespace

namespace ui
{
    OpenFileDialog::OpenFileDialog() = default;

    void OpenFileDialog::OpenAsync()
    {
        // Move data out of the dialog object since it may be destroyed
        callback_t callback(std::move(m_callback));
        std::vector filters(std::move(m_filters));

        // Windows dialogs are not asynchronous -> Spawn another thread
        std::thread(
            [callback, filters]
            {
                const auto initResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
                if (!SUCCEEDED(initResult))
                {
                    callback(DialogCallbackResultType::FAILED, std::nullopt);
                    return;
                }

                auto resultType = DialogCallbackResultType::FAILED;
                std::optional<std::string> result = std::nullopt;
                IFileOpenDialog* pFileOpen;

                const auto hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

                if (SUCCEEDED(hr) && SetFilters(pFileOpen, filters))
                {
                    resultType = ShowFileDialog(pFileOpen, result);
                    pFileOpen->Release();
                }

                CoUninitialize();

                callback(resultType, result);
            })
            .detach();
    }

    SaveFileDialog::SaveFileDialog() = default;

    void SaveFileDialog::OpenAsync()
    {
        // Move data out of the dialog object since it may be destroyed
        callback_t callback(std::move(m_callback));
        std::vector filters(std::move(m_filters));

        // Windows dialogs are not asynchronous -> Spawn another thread
        std::thread(
            [callback, filters]
            {
                const auto initResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
                if (!SUCCEEDED(initResult))
                {
                    callback(DialogCallbackResultType::FAILED, std::nullopt);
                    return;
                }

                auto resultType = DialogCallbackResultType::FAILED;
                std::optional<std::string> result = std::nullopt;
                IFileSaveDialog* pFileSave;

                const auto hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

                if (SUCCEEDED(hr) && SetFilters(pFileSave, filters))
                {
                    resultType = ShowFileDialog(pFileSave, result);
                    pFileSave->Release();
                }

                CoUninitialize();

                callback(resultType, result);
            })
            .detach();
    }

    FolderSelectDialog::FolderSelectDialog() = default;

    void FolderSelectDialog::OpenAsync()
    {
        // Move data out of the dialog object since it may be destroyed
        callback_t callback(std::move(m_callback));

        // Windows dialogs are not asynchronous -> Spawn another thread
        std::thread(
            [callback]
            {
                const auto initResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
                if (!SUCCEEDED(initResult))
                {
                    callback(DialogCallbackResultType::FAILED, std::nullopt);
                    return;
                }

                std::optional<std::string> result = std::nullopt;
                IFileOpenDialog* pFileOpen;

                const auto hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
                if (!SUCCEEDED(hr))
                {
                    CoUninitialize();
                    callback(DialogCallbackResultType::FAILED, std::nullopt);
                    return;
                }

                DWORD dwOptions = 0;
                if (!SUCCEEDED(pFileOpen->GetOptions(&dwOptions)))
                {
                    pFileOpen->Release();
                    CoUninitialize();
                    callback(DialogCallbackResultType::FAILED, std::nullopt);
                    return;
                }

                if (!SUCCEEDED(pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS)))
                {
                    pFileOpen->Release();
                    CoUninitialize();
                    callback(DialogCallbackResultType::FAILED, std::nullopt);
                    return;
                }

                const auto resultType = ShowFileDialog(pFileOpen, result);

                pFileOpen->Release();
                CoUninitialize();

                callback(resultType, result);
            })
            .detach();
    }
} // namespace ui
#endif
