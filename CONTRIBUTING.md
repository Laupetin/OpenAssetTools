# Contributing to OpenAssetTools

Thank you for your interesting in contributing! This project is an effort to make the modding of the classic Call Of
Duty games more open and available to everyone so contributions are welcome.

This file aims to give a bit of guidance on how to get started and what to look out for when trying to make
modifications to this project.

## How can I help?

There are a bunch of ways to contribute to the project. Contributions can be in form of code but can also include
reporting bugs, writing documentation, working on example mods or helping others with questions or problems.

### Bugs

Helping with bugs is always appreciated. Help can be in form of _reporting bugs_ or _fixing them_ directly. In any case,
please include a detailed report on **how to replicate the bug** to be able to ensure that a potential fix works and
solves the issue.

Bugs are tracked as GitHub issues, so feel free to open one if you encounter a bug. You can avoid duplicates if you use
the search to see whether somebody else already reported the problem you are encountering. If you are not sure whether
something is a bug or not, you can always use GitHub discussion or another communication channel to reach out.

### Features

If you want to contribute or request feature, please notice that the upstream repository is opinionated on what will and
should be added as a feature. You are always free to add or modify anything you want in your personal codebase, for
upstream, a lot more use cases and constraints have to kept in mind. (Also I am taking the liberty to have my very own
personal opinion on certain things.)

In terms of "requesting a feature", this means: Please open up
a [GitHub discussion](https://github.com/Laupetin/OpenAssetTools/discussions) about it (or reach out on another
platform) and describe what exactly you are suggesting to be added. Please include some context in how this feature
would help you or what you would gain from having this feature. Sometimes times there might already been an idea or
concept that could tackle a particular use case, that could be discussed there. When a feature is accepted as a
contribution, the discussion can then be converted to an issue, which then is open for implementation.

This also includes cases where you would contribute the feature yourself. Just opening up a pull request might have this
discussion only happen after implementation. That might mean that things would need to be tackled differently or not be
accepted in this state which would be unfortunate if you spent the time on it mainly for upstreaming.

### Documentation

Good documentation helps to share knowledge, new users to get started and enables people to create cool new things.
Keeping it up to date and expanding it is an ongoing effort that is not a small task. Unfortunately (at the point of
time writing this) the project is lacking a lot on documentation. So any efforts and contributions to it are very
appreciated.

There are two types of documentation regarding the project.

The first one is documentation regarding usage of the modding tools, getting started with them. It is directed at
modders of the games and lives in a separate repository: [OpenAssetTools/Docs](https://github.com/OpenAssetTools/Docs).
Its contents are served on the project's website: [https://openassettools.dev/](https://openassettools.dev/). The
contents should include information about getting started, usage and configuration of the different tools and
documentation about the different file formats used by OAT.

Separate to that should be developers documentation that is intended for anyone trying to develop on the project or
modify the tools themselves. It should live in this repository in the [docs](./docs) folder and give more in-depth
information about implementation details, project structure/architecture and development setup. Modders and people using
the tools should not be required to know about it to be able to make use of them.

### Examples

Another helpful resource to people trying to use the project is example projects. While any mod that publishes its
source files (while not being required; big shoutout to anyone doing that - you are awesome) can serve as an example,
there is an official repository containing example projects that intend to demonstrate a variety of common use cases
(reaching from very simple ones to more advanced):
[OpenAssetTools/Examples](https://github.com/OpenAssetTools/Examples).

Contributions to it are also appreciated and help people to get an idea of how to set up a project and do certain
things. Also, if you have a repository yourself that you think could serve great as an example, there is also an option
to link it as a resource.

## Usage Of AI-Tools

This project has a comparatively strict policy regarding AI-tools. This does not mean you cannot use them at all, but
only under some constraints regarding the output that reaches upstream. (You can do what you want in your personal
fork - these rules apply to contributions that reach upstream though.) In the end this is a hobby and fun project and
sifting through AI generated code and text is very tiring and not fun for me at all personally.

- You are entirely responsible for anything that is contributed. Machines cannot take responsibility. It is up to you to
  ensure that the code is good, readable and does what it is supposed to.
- Only contribute what you understand and could have contributed without any AI-tooling as well. You may use these tools
  to speed up the process, help you understand things, give suggestions on implementation or validate your work for
  example. But if you couldn't have done it without these tools because you could otherwise for example not write code
  in the given programming language, or you don't understand **in its entirety**, what the code does, do not contribute
  it.
- Contributions that involved AI tooling should be marked as such, for example with a one-liner in a pull-request.
- Entirely AI generated contributions are not allowed. You must be the main contributor, not the machine.
- Do not generate issues, pull request descriptions or documentation with AI tooling.
- No commit trailers of AI tools.
- The code is primarily intended to be read and understood by humans, so it should look like it as well. Comment when
  necessary but avoid excessive commenting of simple things for example. This of course is a bit subjective, so just
  make sure the code does not aggressively look like it was AI generated.

## License and derivates

The OpenAssetTools project is free software licensed under the [GPLv3 license](./LICENSE).

As a brief but not complete help on what this means (in own words - not as legal advice), here is a list of some notable
points. Anybody acquiring a copy of this project may:

* freely use it, privately or publicly
* make your own modifications to it
* distribute it

under the conditions that

* there is no warranty
* any distribution of this project in any form (including in binary form) must disclose its source code
* any modifications or derivates are to be licensed under GPLv3 as well (which includes the same guarantees as described
  here)
* a copy of the license must be included with its source

By contributing to OpenAssetTools, you agree to license your contribution under its license as well. Please be fair and
grant others the same benefits and knowledge that you could base yours on, so everyone can benefit from shared knowledge
and work.

## Getting In Touch

The main communication platform for getting in touch regarding OAT is GitHub discussions right now. There is no
dedicated Discord server for the project, however I am member in a bunch of CoD modding related servers, so discussions
can also happen there. Additionally, you can also reach out via Discord PM.
