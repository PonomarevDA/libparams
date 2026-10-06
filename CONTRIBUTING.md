# Contributing

## Workflow

The project follows [GitHub Flow](https://docs.github.com/en/get-started/using-github/github-flow):
branch `pr-<topic>` from `main`, open a pull request, it is squash-merged.

Before opening a pull request, run:

```bash
make code_style   # astyle, cpplint, crlf, cppcheck, pylint
make unit_tests   # requires GoogleTest
make ubuntu       # and every platform you touched: stm32f103, stm32g0b1, stm32h753xx
```

## Commits and pull requests

The pull request title becomes the commit subject, so one convention covers both.
A commit message is a single line with no body:

```
<type>(<scope>): <summary>
```

- Write the summary in the imperative, lower case, with no trailing dot.
- `<scope>` is optional and names the affected part: `feat(rom):`, `refactor(ubuntu):`.
- A commit written with an assistant also carries a `Co-authored-by:` trailer, the only line besides the subject.

| Type | Use for |
| ---- | ------- |
| `feat` | New feature or platform |
| `fix` | Bug fix or behavior correction |
| `docs` | Documentation |
| `test` | Tests, test data, test helpers |
| `refactor` | Restructuring without intended behavior change |
| `ci` | Workflows, checks, automation |
| `chore` | Build scripts, tooling, dependencies, repository maintenance |

## Code style

- The core (`src/`, `include/`, MCU drivers) is C11. The SITL driver (`platform_specific/ubuntu`) and tests are C++17.
- Formatting: [astylerc](scripts/code_style/astylerc) (Java braces, 4 spaces, 100 columns) and [CPPLINT.cfg](CPPLINT.cfg).
- Code and comments are in English.
- A comment states a constraint that is not visible from the code: why it must be this way, a hidden coupling, a hardware quirk.
  Do not restate the next line or explain how the code used to work; history lives in git.

## Firmware rules

The library runs in embedded firmware.
Prefer predictable, bounded, explicit code over convenience abstractions.

- No heap in the core and MCU drivers: no `malloc`, `free`, `new`, `delete`, or APIs that hide them (`std::vector`, `std::string`, `std::function`, ...).
  The SITL driver runs on a host and may use the C++ standard library.
- Use fixed-size storage with compile-time capacities.
- Keep execution time bounded: no unbounded loops, blocking waits, sleeps or retries.
- Validate all external input: API arguments, parameter names and values, flash contents (they may be erased or corrupted).
- Keep the public API, macros, CMake variables and the flash layout backward compatible: firmware in the field and downstream repositories depend on them.
  Deprecate first (see the `#pragma message` in [flash_driver.cpp](platform_specific/ubuntu/flash_driver.cpp)), remove later.
- Do not hand-edit generated files. Change the YAML and regenerate.
- Keep dependencies minimal. Do not add libraries without a clear embedded reason.
- Keep logging bounded and off the time-critical paths.

When in doubt, prefer a small explicit implementation over a generic abstraction, compile-time configuration over runtime discovery, and failing safe over continuing with an uncertain state.
Ask before introducing memory allocation, flash layout changes or background execution.

## Docs

Docs are read by busy humans. Keep them short.

- One topic per file.
- Lead with what the reader does. Cut history unless it changes an action.
- Prefer a table, a command or a short list over prose.
- Don't restate what's elsewhere; link to the source of truth.
