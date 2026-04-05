## 📝 Commit Message Conventions

This project follows the [Conventional Commits](https://www.conventionalcommits.org/) standard. This ensures a clean, readable, and automated-tool-friendly git history.

### Format
Each commit message consists of a **type**, an optional **scope**, and a **description**:
`type(scope): description`

### Allowed Types

| Prefix | Description | Example |
| :--- | :--- | :--- |
| **`feat`** | A new feature for the server | `feat(cgi): support POST request body parsing` |
| **`fix`** | A bug or security fix | `fix(cgi): return 403 instead of leaking source code` |
| **`refactor`** | Code change that neither fixes a bug nor adds a feature | `refactor(parser): simplify header extraction logic` |
| **`perf`** | A code change that improves performance | `perf(buffer): reduce memory copies during CGI execution` |
| **`style`** | Changes that do not affect the meaning of the code | `style: fix indentation in main.cpp` |
| **`docs`** | Documentation only changes | `docs: add instructions for running python scripts` |
| **`test`** | Adding missing tests or correcting existing tests | `test: add unit tests for URI decoding` |
| **`build`** | Changes that affect the build system (Makefile, etc.) | `build: add -Wall -Werror flags to Makefile` |
| **`chore`** | Maintenance tasks (updating .gitignore, etc.) | `chore: ignore .DS_Store files` |

### Best Practices
* **Use the imperative mood:** Use "fix" instead of "fixed" or "fixes".
* **Lower case:** Start the description with a lower case letter.
* **No period:** Do not end the description with a period.
* **Breaking Changes:** If a commit breaks backward compatibility, add an `!` after the type (e.g., `feat!: change config file format`).