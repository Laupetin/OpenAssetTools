#!/bin/bash

PREMAKE_URL='https://github.com/premake/premake-core/releases/download/v5.0.0-beta8/premake-5.0.0-beta8-linux.tar.gz'
PREMAKE_HASH='2921a59e267bce1f3e5208b8e04881c94dc23f4849adf0d830875ded23634b1f'

# The following variables can be set:
#     PREMAKE_NO_GLOBAL - Ignore premake5 executable from path
#     PREMAKE_NO_PROMPT - Download premake5 without prompting

function install_premake {
    if [[ ! -x "$(command -v wget)" ]]; then
        echo "Failed: Installation requires wget" >&2
        exit 2
    fi
    if [[ ! -x "$(command -v tar)" ]]; then
        echo "Failed: Installation requires tar" >&2
        exit 2
    fi
    if [[ ! -x "$(command -v sha256sum)" ]]; then
        echo "Failed: Installation requires sha256sum" >&2
        exit 2
    fi

    mkdir -p build
    wget -nd -O build/premake.tar.gz "$PREMAKE_URL"
    if [[ $? -ne 0 ]]; then
        echo "Download failed" >&2
        exit 2
    fi

    tar -xf build/premake.tar.gz -C build
    if [[ $? -ne 0 ]]; then
        echo "Extraction failed" >&2
        exit 2
    fi

    rm build/premake.tar.gz

    echo "${PREMAKE_HASH} build/premake5" | sha256sum -c
    if [[ $? -ne 0 ]]; then
        echo "Hash verification failed" >&2
        rm build/premake5
        exit 2
    fi

    chmod +x build/premake5
}

function expect_inside_git_repository {
    inside_git_repo="$(git rev-parse --is-inside-work-tree 2>/dev/null)"
    if [ ! -d ".git" ] && [ ! "$inside_git_repo" ]; then
        echo "You must clone the OpenAssetTools repository using 'git clone'. Please read README.md." >&2
        exit 1
    fi
}

# Go to repository root
cd "$(dirname "$0")" || exit 2

expect_inside_git_repository

PREMAKE_BIN=''
if [[ -z "$PREMAKE_NO_GLOBAL" ]] && [[ -x "$(command -v premake5)" ]]; then
    PREMAKE_BIN='premake5'
elif [[ -x "$(command -v build/premake5)" ]] && [[ ! -z "$(build/premake5 --version)" ]]; then
    PREMAKE_BIN='build/premake5'
else
    echo "Could not find premake5. You can either install it yourself or this script download it for you."
    if [[ ! -z "$PREMAKE_NO_PROMPT" ]] || [[ "$(read -e -p 'Do you wish to download it automatically? [y/N]> '; echo $REPLY)" == [Yy]* ]]; then
        echo "Installing premake"
        install_premake
        PREMAKE_BIN='build/premake5'
    else
        echo "Please install premake5 and try again"
        exit 1
    fi
fi

git submodule update --init --recursive
$PREMAKE_BIN $@ gmake
