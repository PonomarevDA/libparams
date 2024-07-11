# Contributing to libparams

This project follows the [GitHub Flow](https://docs.github.com/en/get-started/using-github/github-flow) development workflow.

1. Fork and clone the repository
    ```
    git clone git@github.com:<your_github_user_name>/libparams.git --recursive
    ```
2. Create a branch `pr-<branch-name>` from the `main` branch. For a feature the name of your branch can be: `pr-add-smth`, for a fix: `pr-fix-smth`
3. Edit the code
4. Verify your changes with code style checker, linter and unit tests
    ```
    make unit_tests
    make cppcheck
    ```
5. Commit the changes
6. Push your changes via Pull Request. Provide a description of your new feature or fix.
