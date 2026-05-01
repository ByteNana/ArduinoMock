# Contributing to ArduinoNativeMocks

## Prerequisites

| Tool | Version | Install |
|---|---|---|
| CMake | 3.15+ | [cmake.org](https://cmake.org/download/) |
| C++11 compiler | GCC / Clang / MSVC | — |
| clang-format | any recent | `brew install clang-format` |
| Lefthook | any recent | `brew install lefthook` |

## Setup

```bash
git clone <repo-url> && cd ArduinoNativeMocks
make setup          # installs Lefthook hooks + configures CMake
```

## Branching Model

| Branch | Purpose | Push rules |
|---|---|---|
| `master` | Stable releases only | No direct pushes — PR squash-merge only |
| `rc-X.Y.Z` | Release candidate for version X.Y.Z | No force pushes; normal pushes allowed |
| `feature/*`, other | Feature development | No restrictions |

## Development Workflow

1. **Branch** from the current `rc-*` branch (or `master` if no rc exists):
   ```bash
   git checkout rc-0.1.1
   git checkout -b feature/my-change
   ```

2. **Code** — make your changes.

3. **Format** before committing:
   ```bash
   make format        # auto-fix formatting
   make check         # verify (same check the hook runs)
   ```

4. **Commit and push**:
   ```bash
   git add <files>
   git commit -m "feat: description of change"
   git push -u origin feature/my-change
   ```

5. **Open a PR** into the `rc-*` branch.

## Version Bumping

Version is stored in two places in `CMakeLists.txt`:

- `project(... VERSION X.Y.Z ...)` — the major.minor.patch
- `set(PROJECT_VERSION_SUFIX "-rc.N")` — the pre-release suffix (empty on stable releases)

Both fields are updated automatically by CI — no local tooling required.

## Release Process

1. **Create an rc branch** by triggering the **Create Release Candidate** workflow on GitHub Actions with the target stable version (e.g. `0.3.0`). This creates the `rc-0.3.0` branch, bumps `CMakeLists.txt` to `VERSION 0.3.0` / `PROJECT_VERSION_SUFIX "-rc.1"`, and opens a PR to `master`.

2. **Develop on feature branches**, merge PRs into the `rc-*` branch.

3. **When ready to release**, clear the suffix on the rc branch:
   ```bash
   sed -i 's/set(PROJECT_VERSION_SUFIX "[^"]*")/set(PROJECT_VERSION_SUFIX "")/' CMakeLists.txt
   git add CMakeLists.txt
   git commit -m "chore: release 0.3.0"
   ```

4. **Merge the PR** from `rc-0.3.0` → `master`. Squash-merge it.
   > The **Version Check** CI job will fail if the `vX.Y.Z` tag already exists — bump the version in `CMakeLists.txt` before merging.

5. The **auto-tag** GitHub Action creates a `vX.Y.Z` tag and GitHub Release automatically.

## Git Hooks

Lefthook enforces the following hooks:

| Hook | Check | What it does |
|---|---|---|
| `pre-commit` | `format-check` | Runs `make check` on staged `.c/.cpp/.h/.hpp` files |
| `pre-push` | `branch-protection` | Blocks direct pushes to `master`; blocks force pushes to `rc-*` |
| `pre-push` | `version-validation` | On `rc-*` branches, ensures `CMakeLists.txt` VERSION matches the branch name |

### Emergency bypass

```bash
git commit --no-verify    # skip pre-commit hooks
git push --no-verify      # skip pre-push hooks
```

Use sparingly and only when you understand why the hook is failing.

## Code Style

- All C/C++ source files are formatted with **clang-format**
- Run `make format` to auto-format all files in `src/`, `test/`, and `examples/`
- Run `make check` to verify formatting without modifying files
- The `pre-commit` hook runs `make check` automatically
