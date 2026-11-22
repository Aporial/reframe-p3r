# Contributing

Start with [Building](docs/BUILDING.md) and [Architecture](docs/ARCHITECTURE.md). Use a separate branch for each change and keep fixes scoped to a reproducible problem.

- Describe the game build, GPU, driver, backend, mode and exact steps in reports.
- Separate compilation, static analysis and game or GPU results. List the scenarios you ran and the ones that still need manual verification.
- Keep game binaries, shader captures, runtime DLLs, SDKs, save files, machine-specific paths and private logs outside contributions. Share only redacted excerpts for debugging.
- Use the repository dependency lock when testing SDK integrations. Changes to a vendor version need the accompanying checksum and licensing review.

Submit original RE:FRAME code contributions under the project’s [MIT License](LICENSE). Preserve the applicable terms and notices for any third-party material.
